#include "DbMysql.h"
#include "logger/Logger.h"

DbMysql::DbMysql(std::string ip, int port, std::string user, std::string passwd) : ip_(ip), port_(port), user_(user), passwd_(passwd) {

}

DbMysql::~DbMysql() {
	Detach();
}

bool DbMysql::Attach(std::string dbName) {
	mysql_ = mysql_init(0);
	if (!mysql_) {
		LOG_ERROR(std::string("mysql_init error"));
		return false;
	}

	dbName_ = dbName;

	if ( mysql_real_connect(mysql_, ip_.c_str(), user_.c_str(), passwd_.c_str(), dbName_.c_str(), port_, NULL, 0) ) {
		if ( mysql_select_db(mysql_, dbName_.c_str()) != 0 ) {
			LOG_ERROR(fmt::format("select db:{} error", dbName_));
			Detach();
			return false;
		}
	} else {
		LOG_ERROR(fmt::format("connect db:{}:{} user:{},password:{},db:{} error", ip_, port_, user_, passwd_, dbName_));
		Detach();
		return false;
	}

	return true;
}

bool DbMysql::Detach() {
	if (mysql_) {
		mysql_close(mysql_);
		mysql_ = NULL;
	}
	return true;
}

bool DbMysql::Execute(const char* sql, size_t size, MemoryStream& stream) {
	if (!mysql_) {
		LOG_ERROR(std::string("query failed:mysql not init"));
		return false;
	}

	if (mysql_real_query(mysql_, sql, size) != 0) {
		LOG_ERROR(fmt::format("query failed:{} {}({})", sql, mysql_errno(mysql_), mysql_error(mysql_)));
		return false;
	}

	MYSQL_RES* result = mysql_store_result(mysql_);
	if (result) {
		uint32_t nrows = (uint32_t)mysql_num_rows(result);
		uint32_t nfields = (uint32_t)mysql_num_fields(result);

		stream << nfields;
		MYSQL_FIELD* fields = mysql_fetch_fields(result);
		for(uint32_t i = 0;i < nfields;i++) {
			stream << fields[i].name << fields[i].type;
		}

		stream << nrows;
		
		MYSQL_ROW arow;
		while((arow = mysql_fetch_row(result)) != NULL) {
			unsigned long* lengths = (unsigned long*)mysql_fetch_lengths(result);
			for (uint32_t i = 0;i < nfields;i++) {
				if (arow[i] == NULL) {
					stream << (uint32_t)0;
				} else {
					stream << (uint32_t)lengths[i];
					stream.Append((const char*)arow[i], lengths[i]);
					stream << false;
				}
			}

		}		

	} else {
		stream << (uint32_t)mysql_->affected_rows;
	}

	mysql_free_result(result);

	return true;
}

bool DbMysql::GetTableFields(const char* tableName, MemoryStream& stream) {
	MYSQL_RES* result = mysql_list_fields(mysql_, tableName, NULL);
	if (!result) {
		LOG_ERROR(fmt::format("GetTableFields failed:{} {}({})", tableName, mysql_errno(mysql_), mysql_error(mysql_)));
		return false;
	}

	MYSQL_FIELD* fields = mysql_fetch_fields(result);
	uint32_t nfields = mysql_num_fields(result);

	stream << nfields;
	for(uint32_t i = 0;i < nfields;i++) {
		stream << fields[i].name << fields[i].type;
	}
	return true;
}
