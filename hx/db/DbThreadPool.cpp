#include "DbThreadPool.h"
#include <stdio.h>

void DbThreadPool::threadFunc(DbThreadPool::TaskQueue *queue, DbMysql* db) {
	for(;;) {
		DbTask* task = queue->Get();
		if(!task) {
			return;
		}
		task->ThreadDo(db);
	}
}

void DbThreadPool::TaskQueue::Close() {
	std::lock_guard<std::mutex> guard(this->mtx);
	if(!this->closed) {
		this->closed = true;
		this->cv.notify_all();
	}
}

DbTask* DbThreadPool::TaskQueue::Get() {
	std::lock_guard<std::mutex> guard(this->mtx);
	for( ; ;) {
		if(this->closed) {
			if(this->tasks.empty()) {
				return NULL;
			} else {
				DbTask* task = this->tasks.front();
				this->tasks.pop_front();
				return task;
			}
		} else {

			if(this->tasks.empty()) {
				++this->watting;
				this->cv.wait(this->mtx);
				--this->watting;	
			} else {
				DbTask* task = this->tasks.front();
				this->tasks.pop_front();
				return task;
			}	
		}
	}
}


void DbThreadPool::TaskQueue::PostTask(DbTask* task) {
	std::lock_guard<std::mutex> guard(this->mtx);
	if(this->closed) {
		return;
	}	
	this->tasks.push_back(task);
	if(this->watting > 0) {
		this->cv.notify_one();
	}
}

bool DbThreadPool::Init(int threadCount, std::string ip, int port, std::string user, std::string pwd, std::string name) {

	bool expected = false;
	if(threadCount <= 0) {
		threadCount = 1;
	}

	for(auto i = 0; i < threadCount; ++i) {
		DbMysql* db = new DbMysql(ip, port, user, pwd);
		db->Attach(name);
		dbs_.push_back(db);
	}

	for(auto i = 0; i < threadCount; ++i) {
		threads_.push_back(std::thread(threadFunc,&queue_, dbs_[i]));
	}

	return true;
}

DbThreadPool::DbThreadPool() {
}

DbThreadPool::~DbThreadPool() {
	queue_.Close();
	size_t i = 0;
	for( ; i < threads_.size(); ++i) {
		threads_[i].join();
	}

	for( i = 0; i < dbs_.size(); ++i) {
		DbMysql* db = dbs_[i];
		delete db;
	}
}

