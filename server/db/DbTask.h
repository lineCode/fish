#ifndef DB_TASK_H
#define DB_TASK_H
#include "db/DbThreadPool.h"
#include <stdint.h>

class DbQueryTask : public DbTask {
public:
	DbQueryTask(int reference, const char* sql, size_t size);
	virtual ~DbQueryTask();

	virtual void ThreadDo(DbMysql* db);
	virtual void MainDo();

private:
	MemoryStream result_;
	char* sql_;
	size_t size_;
	int reference_;
};

class DbRawSqlTask : public DbTask {
public:
	DbRawSqlTask(int reference, const char* sql, size_t size);
	virtual ~DbRawSqlTask();

	virtual void ThreadDo(DbMysql* db);
	virtual void MainDo();
	
private:
	MemoryStream result_;
	char* sql_;
	size_t size_;
	int reference_;
};

#endif