#ifndef DBTHREADPOOL_H
#define DBTHREADPOOL_H

#include <thread>
#include <vector>
#include <list>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <atomic>




class DbTask {

public:
	typedef std::shared_ptr<DbTask> Ptr;
	virtual void Do() = 0;	
	virtual ~DbTask() {}
};



class DbThreadPool {

	class TaskQueue {

	public:

		TaskQueue():closed(false),watting(0){}

		void PostTask(const DbTask::Ptr &task);

		void Close();

		DbTask::Ptr Get();

		bool Get(std::list<DbTask::Ptr> &out);

	private:
		TaskQueue(const TaskQueue&);
		TaskQueue& operator = (const TaskQueue&);
		bool closed;
		int  watting;
		std::mutex mtx;
		std::condition_variable_any cv;
		std::list<DbTask::Ptr> tasks;
	};

public:

	DbThreadPool(){}

	~DbThreadPool();

	bool Init(int threadCount = 0);

	void PostTask(const DbTask::Ptr &task) {
		queue_.PostTask(task);
	}

	void Close() {
		queue_.Close();
	}

private:

	static void threadFunc(DbThreadPool::TaskQueue *queue_);

	DbThreadPool(const DbThreadPool&);
	DbThreadPool& operator = (const DbThreadPool&);	

	TaskQueue queue_;
	std::vector<std::thread> threads_;
};


#endif

