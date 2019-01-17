#include <unordered_map>
#include <iostream>

#include "logger/Logger.h"
#include "logger/LoggerHost.h"
#include "db/DbMysql.h"

int main() {
	Logger* logger = new Logger(new LoggerHost("./"));
	DbMysql* db = new DbMysql("127.0.0.1", 3306, "root", "123456");
	db->Attach("test");		
	
	return 0;
}
