#include <unordered_map>
#include <iostream>
#include "util/MemoryStream.h"
#include "logger/Logger.h"
#include "logger/LoggerServer.h"
#include "db/DbMysql.h"

int main() {
	Logger* logger = new Logger(new LoggerServer("./"));
	DbMysql* db = new DbMysql("127.0.0.1", 3306, "root", "198932100");
	db->Attach("test");		
	const char* createCmd = "\
			CREATE TABLE IF NOT EXISTS `user`(\
			`userUid` INT UNSIGNED AUTO_INCREMENT,\
			 `name` VARCHAR(100) NOT NULL,\
			 `level` INT UNSIGNED,\
			  PRIMARY KEY ( `userUid` )\
		)ENGINE=InnoDB DEFAULT CHARSET=utf8;";	


	MemoryStream stream;
	for (int i = 1;i < 1024;i++) {
		std::string sql = fmt::format("insert into user (name,level) values ('{}',{})","mrq",i);
		db->Execute(sql.c_str(),sql.size(), stream);

	}


	const char* sql = "select * from user";
	db->Execute(sql, strlen(sql),stream);
	return 0;
}
