#include "LuaHttpParser.h"
#include "http_parser.h"

namespace LuaHttpParser
{
	struct ParserContext {
		struct lua_State* L;
		bool more;
	};

	int ParserBegin(struct http_parser* parser)
	{
		return 0;
	}

	int ParserEnd(struct http_parser* parser)
	{
		ParserContext* ctx = (ParserContext*)parser->data;
		ctx->more = false;
		return 0;
	}

	int OnHeaderComplete(struct http_parser* parser)
	{
		return 0;
	}

	int OnUrl(struct http_parser* parser, const char *at, size_t length)
	{
		ParserContext* ctx = (ParserContext*)parser->data;
		lua_pushlstring(ctx->L,at,length);
		lua_setfield(ctx->L,-2,"url");
		return 0;
	}

	int OnState(struct http_parser* parser, const char *at, size_t length)
	{
		ParserContext* ctx = (ParserContext*)parser->data;
		lua_pushlstring(ctx->L,at,length);
		lua_setfield(ctx->L,-2,"state");
		return 0;
	}

	int OnHeaderField(struct http_parser* parser, const char *at, size_t length)
	{
		ParserContext* ctx = (ParserContext*)parser->data;
		lua_pushlstring(ctx->L,at,length);
		return 0;
	}

	int OnHeaderValue(struct http_parser* parser, const char *at, size_t length)
	{
		ParserContext* ctx = (ParserContext*)parser->data;
		lua_pushlstring(ctx->L,at,length);
		lua_settable(ctx->L,-3);
		return 0;
	}

	int OnBody(struct http_parser* parser, const char *at, size_t length)
	{
		ParserContext* ctx = (ParserContext*)parser->data;
		lua_pushlstring(ctx->L,at,length);
		lua_setfield(ctx->L,-2,"body");
		return 0;
	}

	int ParserRequest(lua_State* L)
	{
		size_t total = 0;
		const char* data = luaL_checklstring(L,1,&total);

		ParserContext parserCtx;
		parserCtx.L = L;
		parserCtx.more = true;
		struct http_parser parser;

		http_parser_init(&parser, HTTP_REQUEST);
		parser.data = &parserCtx;

		lua_createtable(L,0,0);
		http_parser_settings settings;
		settings.on_message_begin = ParserBegin;
		settings.on_url = OnUrl;
		settings.on_status = OnState;
		settings.on_header_field = OnHeaderField;
		settings.on_header_value = OnHeaderValue;
		settings.on_headers_complete = OnHeaderComplete;
		settings.on_body = OnBody;
		settings.on_message_complete = ParserEnd;

		int len = http_parser_execute(&parser, &settings, data, total);

		int rargs = 2;
		lua_pushboolean(L,parserCtx.more);
		if (!parserCtx.more)
		{
			if (total - len > 0)
			{
				lua_pushlstring(L,data + len,total- len);
				rargs = 3;
			}
		}
		return rargs;
	}

	int ParserUrl(lua_State* L)
	{
		return 1;
	}

	int Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "ParserRequest" ,ParserRequest },
			{ "ParserUrl", ParserUrl },
			{ NULL, NULL },
		};

		luaL_newlibtable(L, l);
		luaL_setfuncs(L,l,0);

		return 1;
	}
}