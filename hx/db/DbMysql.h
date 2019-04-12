#ifndef DB_MYSQL_H
#define DB_MYSQL_H
#define WIN32_LEAN_AND_MEAN
#include <mysql/mysql.h>
#include <string>
#include "util/MemoryStream.h"

class DbMysql {
public:
	DbMysql(std::string ip, int port, std::string user, std::string passwd);
	virtual ~DbMysql();

	bool Attach(std::string dbName);
	bool Detach();

	bool Execute(const char* cmd, size_t size, MemoryStream& stream);

	bool GetTableFields(const char* tableName, MemoryStream& stream);
private:
	std::string dbName_;
	std::string ip_;
	int port_;
	std::string user_;
	std::string passwd_;
	MYSQL* mysql_;
};

#endif
