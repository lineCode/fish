#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "LuaProf.h"

#ifdef WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#endif

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#ifdef __cplusplus
#define EXTERN_BEGIN extern "C" { EXPORT
#define EXTERN_END }
#else
#define EXTERN_BEGIN
#define EXTERN_END
#endif

namespace LuaProf
{
	struct profile_function
	{
		int init;
		double start_time;
		double total_cost;
		double max_cost;
		int count;
		int parent;
		int fullname;
		int name;
		int what;
		int source;
		int linedefined;
		int currentline;
	};

	struct profile_stack
	{
		struct profile_stack* next;
		struct profile_function* func_info;
		int tailcall;
		int deep;
	};

	struct profile_func_state
	{
		struct profile_func_state* next;
		struct lua_State* L;
	};

	struct profile_thread
	{
		struct profile_thread* prev;
		struct profile_thread* next;
		struct lua_State* L;
		struct profile_stack* stack;
		struct profile_func_state* func_state;
		int deep;
		int yield;
	};

	struct profile_ctx
	{
		struct profile_thread* head;
		struct profile_thread* tail;
		struct profile_func_state* func_state_head;
		struct lua_State* string_map;//string map
		int string_index;
	};

	static double timestamp_persecond()
	{
#if defined (WIN32)
		LARGE_INTEGER rate;
		QueryPerformanceFrequency(&rate);
		return rate.QuadPart;
#else
		return 1000000ULL;
#endif
	}

	static double timestamp()
	{
#if defined (WIN32)
		LARGE_INTEGER counter;
		QueryPerformanceCounter( &counter );
		return counter.QuadPart;
#else
		struct timeval tv;
		gettimeofday( &tv, NULL );
		return 1000000ULL * tv.tv_sec + tv.tv_usec;
#endif
	}

	static void record_time(struct profile_function* func)
	{
		func->start_time = timestamp();
	}

	static void calc_time(struct profile_function* func)
	{
		double cost = timestamp() - func->start_time;
		func->total_cost += cost;
		func->count++;

		if (func->max_cost < cost)
			func->max_cost = cost;
	}

	static int stringindex(struct profile_ctx *ctx, const char * str, size_t sz)
	{
		lua_State *L = ctx->string_map;
		lua_pushlstring(L, str, sz);
		lua_pushvalue(L, -1);
		lua_rawget(L, 1);
		int index;

		if (lua_isnil(L, -1))
		{
			index = ++ctx->string_index;
			lua_pop(L, 1);
			lua_pushinteger(L, index);
			lua_rawset(L, 1);
		} 
		else 
		{
			index = lua_tointeger(L, -1);
			lua_pop(L, 2);
		}
		return index;
	}

	static int concat_fullname(struct lua_State* L)
	{
		luaL_Buffer buff;
		luaL_buffinit(L, &buff);

		size_t sourcelen;
		const char* source = lua_tolstring(L,1,&sourcelen);

		size_t namelen;
		const char* name = lua_tolstring(L,2,&namelen);

		int defineline = lua_tointeger(L,3);
		int currentline = lua_tointeger(L,4);

		char defineline_str[32];
		sprintf(defineline_str, "%d", defineline);
		char currentline_str[32];
		sprintf(currentline_str, "%d", currentline);

		const char* dot = "#";
		luaL_addlstring(&buff,source,sourcelen);
		luaL_addlstring(&buff,dot,strlen(dot));
		luaL_addlstring(&buff,name,namelen);
		luaL_addlstring(&buff,dot,strlen(dot));
		luaL_addlstring(&buff,defineline_str,strlen(defineline_str));
		luaL_addlstring(&buff,dot,strlen(dot));
		luaL_addlstring(&buff,currentline_str,strlen(currentline_str));

		luaL_pushresult(&buff);
		return 1;
	}

	static struct profile_function *get_funcinfo(struct profile_func_state* func_state,const char* func_name)
	{
		lua_State* L = func_state->L;
		lua_getfield(L,1,func_name);
		if (lua_isnil(L,-1))
		{
			lua_pop(L,1);
			struct profile_function* func_info = (struct profile_function*)malloc(sizeof(*func_info));
			memset(func_info,0,sizeof(*func_info));
			lua_pushstring(L,func_name);
			lua_pushlightuserdata(L,func_info);
			lua_settable(L,1);
			return func_info;
		}

		struct profile_function* func_info = (struct profile_function*)lua_touserdata(L,-1);
		lua_pop(L,1);
		return func_info;
	}

	static int coroutine_create(lua_State* L)
	{
		lua_pushstring(L,"ctx");
		lua_gettable(L,LUA_REGISTRYINDEX);
		struct profile_ctx* ctx = (struct profile_ctx*)lua_touserdata(L,-1);
		lua_pop(L,1);

		struct profile_thread* thr = (struct profile_thread*)malloc(sizeof(*thr));
		memset(thr,0,sizeof(*thr));
		thr->prev = thr->next = NULL;
		thr->deep = 0;
		thr->func_state = (struct profile_func_state*)malloc(sizeof(*thr->func_state));
		thr->func_state->next = NULL;
		thr->func_state->L = luaL_newstate();
		lua_settop(thr->func_state->L,0);
		lua_createtable(thr->func_state->L,0,0);

		ctx->tail->next = thr;
		thr->prev = ctx->tail;
		ctx->tail = thr;

		lua_CFunction co_resume = lua_tocfunction(L, lua_upvalueindex(1));

		co_resume(L);

		lua_State* NL = lua_tothread(L,-1);
		lua_pushlightuserdata(NL, NL);
		lua_pushlightuserdata(NL, thr);
		lua_settable(NL,LUA_REGISTRYINDEX);

		thr->L = NL;

		return 1;
	}

	static int coroutine_resume(lua_State* L)
	{
		lua_State* RL = lua_tothread(L,1);
		lua_pushlightuserdata(L,RL);
		lua_gettable(L,LUA_REGISTRYINDEX);
		struct profile_thread* thr = (struct profile_thread*)lua_touserdata(L,-1);
		lua_pop(L,1);

		double now = timestamp();

		struct profile_stack* stack = thr->stack;
		while (stack != NULL)
		{
			stack->func_info->start_time = now;
			stack = stack->next;
		}

		thr->yield = 0;

		lua_CFunction co_resume = lua_tocfunction(L, lua_upvalueindex(1));
		return co_resume(L);
	}

	static int coroutine_yield(lua_State *L) 
	{
		lua_pushlightuserdata(L,L);
		lua_gettable(L,LUA_REGISTRYINDEX);
		struct profile_thread* thr = (struct profile_thread*)lua_touserdata(L,-1);
		lua_pop(L,1);

		double now = timestamp();

		struct profile_stack* stack = thr->stack;
		while (stack != NULL)
		{
			double cost = now - stack->func_info->start_time;
			stack->func_info->total_cost += cost;
			stack = stack->next;
		}
		thr->yield = 1;

		lua_CFunction co_yield = lua_tocfunction(L, lua_upvalueindex(1));
		return co_yield(L);
	}

	static void callhook(lua_State *L, lua_Debug *ar)
	{
		lua_pushstring(L,"ctx");
		lua_gettable(L,LUA_REGISTRYINDEX);
		struct profile_ctx* ctx = (struct profile_ctx*)lua_touserdata(L,-1);
		lua_pop(L,1);

		lua_pushlightuserdata(L,L);
		lua_gettable(L,LUA_REGISTRYINDEX);
		struct profile_thread* thr = (struct profile_thread*)lua_touserdata(L,-1);
		lua_pop(L,1);

		int currentline;
		lua_Debug previous_ar;

		if (lua_getstack(L, 1, &previous_ar) == 0) 
			currentline = -1;
		else 
		{
			lua_getinfo(L, "l", &previous_ar);
			currentline = previous_ar.currentline;
		}
		lua_getinfo(L, "nS", ar);

		if (ar->event == LUA_HOOKCALL || ar->event == LUA_HOOKTAILCALL)
		{
			struct profile_stack* stack = (struct profile_stack*)malloc(sizeof(*stack));
			memset(stack,0,sizeof(*stack));

			lua_pushcfunction(ctx->string_map,concat_fullname);
			lua_pushstring(ctx->string_map,ar->source);

			if (ar->event == LUA_HOOKTAILCALL)
			{
				stack->tailcall = 1;
				lua_pushstring(ctx->string_map,"tailcall");
			}
			else if (ar->name == NULL)	
				lua_pushstring(ctx->string_map,"unknown");
			else
				lua_pushstring(ctx->string_map,ar->name);

			lua_pushinteger(ctx->string_map,ar->linedefined);
			lua_pushinteger(ctx->string_map,currentline);

			if (lua_pcall(ctx->string_map,4,1,0) != LUA_OK)
				luaL_error(ctx->string_map,"concat_fullname:%s\n",lua_tostring(ctx->string_map,-1));

			size_t fullname_len;
			const char* fullname = lua_tolstring(ctx->string_map,-1,&fullname_len);
			lua_settop(ctx->string_map,1);

			stack->func_info = get_funcinfo(thr->func_state,fullname);

			if (!stack->func_info->init)
			{
				if (ar->event == LUA_HOOKTAILCALL)
					stack->func_info->name = stringindex(ctx,"tailcall",strlen("tailcall"));
				else if (ar->name == NULL)	
					stack->func_info->name = stringindex(ctx,"unknown",strlen("unknown"));
				else
					stack->func_info->name = stringindex(ctx,ar->name,strlen(ar->name));

				stack->func_info->what = stringindex(ctx,ar->what,strlen(ar->what));
				stack->func_info->source = stringindex(ctx,ar->source,strlen(ar->source));
				stack->func_info->linedefined = ar->linedefined;
				stack->func_info->currentline = currentline;
				stack->func_info->fullname = stringindex(ctx,fullname,fullname_len);
				stack->func_info->init = 1;

				if (thr->stack != NULL)
					stack->func_info->parent = thr->stack->func_info->fullname;
			}

			record_time(stack->func_info);
			stack->next = thr->stack;
			thr->stack = stack;
			stack->deep = thr->deep++;

			// printf("%d,%x,%s\n",stack->deep,(void*)L,fullname);
		}
		else if (ar->event == LUA_HOOKRET)
		{
			if (thr->stack == NULL)
				printf("profiler just\n");
			else
			{
				struct profile_stack* stack = thr->stack;
				calc_time(stack->func_info);

				if (stack->tailcall)
				{
					while (stack->tailcall == 1)
					{
						struct profile_stack* tmp = stack;
						stack = stack->next;
						calc_time(stack->func_info);
						thr->deep--;
						free(tmp);
					}
					thr->stack = stack;
				}
				thr->stack = thr->stack->next;
				thr->deep--;
				free(stack);

				if (thr->stack == NULL && thr != ctx->head)
				{
					//thread dead,free it
					thr->prev->next = thr->next;
					if (thr->next != NULL)
						thr->next->prev = thr->prev;
					if (thr == ctx->tail)
						ctx->tail = thr->prev;

					thr->func_state->next = ctx->func_state_head;
					ctx->func_state_head = thr->func_state;

					free(thr);
				}
			}
		}
		else
			printf("ar->event:%d\n",ar->event);
	}

	static int profile_start(lua_State* L)
	{
		struct profile_ctx* ctx = (struct profile_ctx*)malloc(sizeof(*ctx));
		memset(ctx,0,sizeof(*ctx));

		ctx->string_map = luaL_newstate();
		ctx->string_index = 0;

		struct profile_thread* thr = (struct profile_thread*)malloc(sizeof(*thr));
		memset(thr,0,sizeof(*thr));
		thr->prev = thr->next = NULL;
		thr->deep = 0;
		thr->L = L;

		thr->func_state = (struct profile_func_state*)malloc(sizeof(*thr->func_state));
		thr->func_state->next = NULL;
		thr->func_state->L = luaL_newstate();
		lua_settop(thr->func_state->L,0);
		lua_createtable(thr->func_state->L,0,0);

		ctx->head = ctx->tail = thr;

		luaL_openlibs(ctx->string_map);
		lua_settop(ctx->string_map,0);
		lua_createtable(ctx->string_map,0,0);

		lua_pushstring(L, "ctx");
		lua_pushlightuserdata(L, ctx);
		lua_settable(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L,L);
		lua_pushlightuserdata(L, thr);
		lua_settable(L, LUA_REGISTRYINDEX);

		//hook coroutine
		lua_getglobal(L,"coroutine");

		lua_getfield(L,-1,"create");
		lua_CFunction co_create = lua_tocfunction(L, -1);
		if (co_create == NULL)
			return luaL_error(L, "can't get coroutine.create");
		lua_pop(L,1);
		lua_pushcfunction(L,co_create);
		lua_pushcclosure(L,coroutine_create,1);
		lua_setfield(L,-2,"create");

		lua_getfield(L,-1,"resume");
		lua_CFunction co_resume = lua_tocfunction(L, -1);
		if (co_resume == NULL)
			return luaL_error(L, "can't get coroutine.resume");
		lua_pop(L,1);
		lua_pushcfunction(L,co_resume);
		lua_pushcclosure(L,coroutine_resume,1);
		lua_setfield(L,-2,"resume");

		lua_getfield(L,-1,"yield");
		lua_CFunction co_yield = lua_tocfunction(L, -1);
		if (co_yield == NULL)
			return luaL_error(L, "can't get coroutine.yield");
		lua_pop(L,1);
		lua_pushcfunction(L,co_yield);
		lua_pushcclosure(L,coroutine_yield,1);
		lua_setfield(L,-2,"yield");

		lua_pop(L,1);

		lua_sethook(L, (lua_Hook)callhook, LUA_MASKCALL | LUA_MASKRET, 0);

		return 0;
	}

	static int profile_over(lua_State* L)
	{
		int dump = 0;
		if (lua_gettop(L) != 0)
		{
			luaL_checktype(L, 1, LUA_TBOOLEAN);
			dump = lua_toboolean(L,1);
		}

		lua_pushstring(L,"ctx");
		lua_gettable(L,LUA_REGISTRYINDEX);
		struct profile_ctx* ctx = (struct profile_ctx*)lua_touserdata(L,-1);
		lua_pop(L,1);

		lua_checkstack(ctx->string_map, ctx->string_index + LUA_MINSTACK);
		lua_settop(ctx->string_map, ctx->string_index + 1);
		lua_pushvalue(ctx->string_map, 1);

		lua_pushnil(ctx->string_map);
		while (lua_next(ctx->string_map, -2) != 0)
		{
			int idx = lua_tointeger(ctx->string_map, -1);
			lua_pop(ctx->string_map, 1);
			lua_pushvalue(ctx->string_map, -1);
			lua_replace(ctx->string_map, idx);
		}
		lua_pop(ctx->string_map, 1);

		struct profile_thread* tmpthr = ctx->head;
		while(tmpthr != NULL)
		{	
			tmpthr->func_state->next = ctx->func_state_head;
			ctx->func_state_head = tmpthr->func_state;
			tmpthr = tmpthr->next;
		}


		FILE* file = fopen("prof.csv","w");
		fprintf(file,"id,function,parent,type,source,define-line,invoke-line,invoke-count,total-cost,averg-cost,max-cost\r\n");

		struct profile_func_state* func_state = ctx->func_state_head;
		int index = 0;
		while(func_state != NULL)
		{	
			lua_pushnil(func_state->L);
			while (lua_next(func_state->L, -2) != 0)
			{
				struct profile_function* func = (struct profile_function*)lua_touserdata(func_state->L,-1);
				const char* parent = NULL;
				if (func->parent != 0)
					parent = lua_tostring(ctx->string_map,func->parent);

				const char* name = lua_tostring(ctx->string_map,func->fullname);
				const char* what = lua_tostring(ctx->string_map,func->what);
				const char* source = lua_tostring(ctx->string_map,func->source);

				if (dump)
					printf("%s %s %s %s %d %d %d %fs %fs %fs\r\n",name,parent,what,source,func->linedefined,func->currentline,func->count,func->total_cost/timestamp_persecond(),func->total_cost/timestamp_persecond()/func->count,func->max_cost/timestamp_persecond());

				fprintf(file,"%d,%s,%s,%s,%s,%d,%d,%d,%f,%f,%f\r\n",index++,name,parent,what,source,func->linedefined,func->currentline,func->count,func->total_cost/timestamp_persecond(),func->total_cost/timestamp_persecond()/func->count,func->max_cost/timestamp_persecond());
				free(func);//free function info

				lua_pop(func_state->L, 1);
			}
			lua_pop(func_state->L, 1);
			lua_close(func_state->L);

			struct profile_func_state* next = func_state->next;
			free(func_state);
			func_state = next;
		}

		fclose(file);

		//free func map
		lua_close(ctx->string_map);

		//remove ctx
		lua_pushstring(L,"ctx");
		lua_pushnil(L);
		lua_settable(L, LUA_REGISTRYINDEX);

		struct profile_thread* thr = ctx->head;
		while(thr != NULL)
		{	
			lua_sethook(thr->L, NULL, 0, 0);

			struct profile_stack* stack = thr->stack;
			while(stack != NULL)
			{
				struct profile_stack* tmp = stack;
				stack = stack->next;
				free(tmp);//free profile_stack
			}
			struct profile_thread* tmp = thr;
			thr = thr->next;
			free(tmp);//free profile_thread
		}

		free(ctx);//free ctx

		//recover coroutine
		lua_getglobal(L,"coroutine");

		lua_getfield(L,-1,"create");
		lua_getupvalue(L,-1,1);
		lua_setfield(L,-3,"create");
		lua_pop(L,1);

		lua_getfield(L,-1,"resume");
		lua_getupvalue(L,-1,1);
		lua_setfield(L,-3,"resume");
		lua_pop(L,1);

		lua_getfield(L,-1,"yield");
		lua_getupvalue(L,-1,1);
		lua_setfield(L,-3,"yield");
		lua_pop(L,1);

		lua_pop(L,1);

		return 0;
	}

	int luaopen_prof(lua_State *L)
	{
		luaL_checkversion(L);
		luaL_Reg l[] ={
			{ "start", profile_start },
			{ "over", profile_over },
			{ NULL, NULL },
		};

		luaL_newlib(L,l);

		return 1;
	}


}
