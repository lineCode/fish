#ifndef DBTHREADPOOL_H
#define DBTHREADPOOL_H

#include <thread>
#include <vector>
#include <list>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <atomic>




class ThreadTask {

public:
	typedef std::shared_ptr<ThreadTask> Ptr;
	virtual void ThreadDo() = 0;	
	virtual ~ThreadTask() {}
};



class DbThreadPool {

	class TaskQueue {

	public:

		TaskQueue():closed(false),watting(0){}

		void PostTask(const ThreadTask::Ptr &task);

		void Close();

		ThreadTask::Ptr Get();

		bool Get(std::list<ThreadTask::Ptr> &out);

	private:
		TaskQueue(const TaskQueue&);
		TaskQueue& operator = (const TaskQueue&);
		bool closed;
		int  watting;
		std::mutex mtx;
		std::condition_variable_any cv;
		std::list<ThreadTask::Ptr> tasks;
	};

public:

	DbThreadPool(){}

	~DbThreadPool();

	bool Init(int threadCount = 0);

	void PostTask(const ThreadTask::Ptr &task) {
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

