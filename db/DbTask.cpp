#include "DbTask.h"
#include "DbApp.h"
#include "logger/Logger.h"

#ifdef WIN32
#define strdup _strdup
#endif
DbQueryTask::DbQueryTask(int reference, const char* sql, size_t size) {
	sql_ = strdup(sql);
	size_ = size;
	reference_ = reference;
}

DbQueryTask::~DbQueryTask() {
	if (sql_) {
		free(sql_);
	}
}

void DbQueryTask::ThreadDo(DbMysql* db) {
	db->Execute(sql_, size_, result_);
	APP->GetQueue()->Push(this);
}

void DbQueryTask::MainDo() {
	uint32_t fieldCount;
	result_ >> fieldCount;

	std::vector<std::string> names;
	std::vector<uint32_t> types;

	for (uint32_t i = 0;i < fieldCount;i++) {
		std::string name;
		uint32_t type;
		result_ >> name >> type;
		names.push_back(name);
		types.push_back(type);
	}

	uint32_t rowCount;
	result_ >> rowCount;

	lua_State* L = APP->Lua()->GetScript().state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
	lua_createtable(L, rowCount, 0);

	for(uint32_t i = 0;i < rowCount;i++) {
		lua_createtable(L, 0, fieldCount);
		for(uint32_t j = 0;j < fieldCount;j++) {
			uint32_t length;
			result_ >> length;
			if (length == 0) {
				continue;
			}

			int offset = result_.ReadOffset();
			char* data = result_.Peek(length);
			result_.ReadOffset(offset+length+1);

			uint32_t type = types[j];

			switch (type) {
				case MYSQL_TYPE_VAR_STRING: case MYSQL_TYPE_STRING: case MYSQL_TYPE_BLOB: 
				case MYSQL_TYPE_TINY_BLOB: case MYSQL_TYPE_MEDIUM_BLOB: case MYSQL_TYPE_LONG_BLOB: {
					lua_pushlstring(L, data, length);
					break;
				}
				case MYSQL_TYPE_SHORT: case MYSQL_TYPE_LONG: case MYSQL_TYPE_LONGLONG:
				case MYSQL_TYPE_INT24: case MYSQL_TYPE_TINY: case MYSQL_TYPE_YEAR: {
					lua_Integer val = atol(data);
					lua_pushinteger(L, val);	
					break;
				}
				case MYSQL_TYPE_DECIMAL: case MYSQL_TYPE_FLOAT: case MYSQL_TYPE_DOUBLE: {
					lua_Number val = atof(data);
					lua_pushnumber(L, val);
					break;
				}
				case MYSQL_TYPE_DATE: case MYSQL_TYPE_NEWDATE: case MYSQL_TYPE_DATETIME:
				case MYSQL_TYPE_TIME: case MYSQL_TYPE_TIMESTAMP: {
					lua_pushlstring(L, data, length);
					break;
				}
				case MYSQL_TYPE_NULL: {
					continue;
				}
				default: {
					lua_pushlstring(L, data, length);
					break;
				}
			}

			std::string& name = names[j];
			lua_setfield(L, -2, name.c_str());
		}

		lua_rawseti(L, -2, i + 1);
	}

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("DbQueryTask::MainDo error:{}", lua_tostring(L, -1)));
	}

	luaL_unref(L, LUA_REGISTRYINDEX, reference_);
}

DbRawSqlTask::DbRawSqlTask(int reference, const char* sql, size_t size) {
	sql_ = strdup(sql);
	size_ = size;
	reference_ = reference;
}

DbRawSqlTask::~DbRawSqlTask() {
	if (sql_) {
		free(sql_);
	}
}

void DbRawSqlTask::ThreadDo(DbMysql* db) {
	db->Execute(sql_, size_, result_);
	APP->GetQueue()->Push(this);
}

void DbRawSqlTask::MainDo() {
	uint32_t affectedRows;
	result_ >> affectedRows;

	lua_State* L = APP->Lua()->GetScript().state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
	lua_pushinteger(L, affectedRows);

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("DbRawSqlTask::MainDo error:{}", lua_tostring(L, -1)));
	}

	luaL_unref(L, LUA_REGISTRYINDEX, reference_);
}
