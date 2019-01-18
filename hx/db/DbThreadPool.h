#ifndef DBTHREADPOOL_H
#define DBTHREADPOOL_H

#include <thread>
#include <vector>
#include <list>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <atomic>
#include "DbMysql.h"
#include "TaskQueue.h"


class DbTask : public MainTask {
public:
	virtual void ThreadDo(DbMysql* db) = 0;	
	virtual ~DbTask() {}
};



class DbThreadPool {

	class TaskQueue {

	public:

		TaskQueue():closed(false),watting(0){}

		void PostTask(DbTask* task);

		void Close();

		DbTask::Ptr Get();
	private:
		TaskQueue(const TaskQueue&);
		TaskQueue& operator = (const TaskQueue&);
		bool closed;
		int  watting;
		std::mutex mtx;
		std::condition_variable_any cv;
		std::list<DbTask*> tasks;
	};

public:

	DbThreadPool();

	~DbThreadPool();

	bool Init(int threadCount, std::string ip, int port, std::string user, std::string pwd);

	void PostTask(DbTask* task) {
		queue_.PostTask(task);
	}

	void Close() {
		queue_.Close();
	}

private:

	static void threadFunc(DbThreadPool::TaskQueue *queue_, DbMysql* db);

	DbThreadPool(const DbThreadPool&);
	DbThreadPool& operator = (const DbThreadPool&);	

	TaskQueue queue_;
	std::vector<std::thread> threads_;
	std::vector<DbMysql*> dbs_;
};


#endif

