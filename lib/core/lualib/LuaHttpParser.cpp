#include "LuaHttpParser.h"
#include "http_parser.h"

namespace LuaHttpParser
{
	int ParserBegin(struct http_parser* parser)
	{
		return 0;
	}

	int ParserEnd(struct http_parser* parser)
	{
		return 0;
	}

	int OnHeaderComplete(struct http_parser* parser)
	{
		return 0;
	}

	int OnUrl(struct http_parser* parser, const char *at, size_t length)
	{
		lua_pushlstring((lua_State*)parser->data,at,length);
		lua_setfield((lua_State*)parser->data,-2,"url");
		return 0;
	}

	int OnState(struct http_parser* parser, const char *at, size_t length)
	{
		lua_pushlstring((lua_State*)parser->data,at,length);
		lua_setfield((lua_State*)parser->data,-2,"state");
		return 0;
	}

	int OnHeaderField(struct http_parser* parser, const char *at, size_t length)
	{
		lua_pushlstring((lua_State*)parser->data,at,length);
		return 0;
	}

	int OnHeaderValue(struct http_parser* parser, const char *at, size_t length)
	{
		lua_pushlstring((lua_State*)parser->data,at,length);
		lua_settable((lua_State*)parser->data,-3);
		return 0;
	}

	int OnBody(struct http_parser* parser, const char *at, size_t length)
	{
		lua_pushlstring((lua_State*)parser->data,at,length);
		lua_setfield((lua_State*)parser->data,-2,"body");
		return 0;
	}

	int ParserRequest(lua_State* L)
	{
		size_t total = 0;
		const char* data = luaL_checklstring(L,1,&total);

		struct http_parser parser;
		http_parser_init(&parser, HTTP_REQUEST);
		parser.data = L;

		lua_createtable(L,0,0);

		struct http_parser_settings setting;
		setting.on_message_begin = ParserBegin;
		setting.on_url = OnUrl;
		setting.on_status = OnState;
		setting.on_header_field = OnHeaderField;
		setting.on_header_value = OnHeaderValue;
		setting.on_headers_complete = OnHeaderComplete;
		setting.on_body = OnBody;
		setting.on_message_complete = ParserEnd;

		http_parser_execute(&parser, &setting, data, total);

		return 1;
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