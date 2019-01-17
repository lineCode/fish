#include "DbMysql.h"
#include "logger/Logger.h"

DbMysql::DbMysql(std::string ip, int port, std::string user, std::string passwd) : ip_(ip), port_(port), user_(user), passwd_(passwd) {

}

DbMysql::~DbMysql() {

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

}

bool DbMysql::Detach() {
	if (mysql_) {
		mysql_close(mysql_);
		mysql_ = NULL;
	}
	return true;
}

bool DbMysql::Query(const char* cmd, size_t size, MemoryStream& stream) {
	if (!mysql_) {
		LOG_ERROR(std::string("query failed:mysql not init"));
		return false;
	}

	int result = mysql_real_query(mysql_, cmd, size);
	if (result != 0) {
		LOG_ERROR(fmt::format("query failed:{} {}({})", cmd, mysql_errno(mysql_), mysql_error(mysql_)));
		return false;
	}

	MYSQL_RES* res = mysql_store_result(mysql_);
	if (res) {
		uint32_t nrows = (uint32_t)mysql_num_rows(res);
		uint32_t nfields = (uint32_t)mysql_num_fields(res);

		stream << nfields;
		MYSQL_FIELD* fields = mysql_fetch_fields(res);
		for(int i = 0;i < nfields;i++) {
			stream << fields[i].name << fields[i].type;
		}

		stream << nrows;
		
		MYSQL_ROW arow;
		while((arow = mysql_fetch_row(res)) != NULL) {
			unsigned long* lengths = mysql_fetch_lengths(res);
			for (uint32_t i = 0;i < nfields;i++) {
				if (arow[i] == NULL) {
					stream << "null";
				} else {
					stream.Append((const char*)arow[i], lengths[i]);
				}
			}

		}		

	} else {
		stream << (uint32_t)mysql_->affected_rows;
	}

}

bool DbMysql::Execute(const char* cmd, size_t size, MemoryStream& stream) {
	return false;
}

bool DbMysql::GetTableFields(const char* tableName, MemoryStream& stream) {
	return false;
}
