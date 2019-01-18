#ifndef DB_TASK_H
#define DB_TASK_H
#include "TaskQueue.h"
#include "DbThreadPool.h"
class DbTask : public ThreadTask, public MainTask {
public:
	DbTask() {} 
	virtual ~DbTask() {}

	virtual void ThreadDo() {}

	virtual void MainDo() {}
};

class DbQueryTask : public DbTask {

};

class DbRawSqlTask : public DbTask {

};


#endif
