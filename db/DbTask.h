#ifndef DB_TASK_H
#define DB_TASK_H

class DbQueryTask : public DbTask {
public:
	DbQueryTask(const char* sql, size_t size);
	virtual ~DbQueryTask();

	virtual void ThreadDo(DbMysql* db);
	virtual void MainDo();

private:
	MemoryStream result_;
	char* sql_;
	size_t size_;
};

class DbRawSqlTask : public DbTask {
public:
	DbRawSqlTask(const char* sql, size_t size);
	virtual ~DbRawSqlTask();

	virtual void ThreadDo(DbMysql* db);
	virtual void MainDo();
	
private:
	MemoryStream result_;
	char* sql_;
	size_t size_;
};

#endif