#include "DbThreadPool.h"
#include <stdio.h>

void DbThreadPool::threadFunc(DbThreadPool::TaskQueue *queue_) {
	std::list<DbTask::Ptr> local_tasks;
	for(;;) {
		if(!queue_->Get(local_tasks)) {
			return;
		}
		for( ; !local_tasks.empty() ; ) {
			const DbTask::Ptr &task = local_tasks.front();
			task->Do();
			local_tasks.pop_front();
		}
	}

/*
	for(;;) {
		DbTask::Ptr task = queue_->Get();
		if(task == nullptr) {
			return;
		}
		task->Do();
	}
*/
}

void DbThreadPool::TaskQueue::Close() {
	std::lock_guard<std::mutex> guard(this->mtx);
	if(!this->closed) {
		this->closed = true;
		this->cv.notify_all();
	}
}

bool DbThreadPool::TaskQueue::Get(std::list<DbTask::Ptr> &out) {
	std::lock_guard<std::mutex> guard(this->mtx);
	for( ; ;) {
		if(this->closed) {
			if(this->tasks.empty()) {
				return false;
			} else {
				this->tasks.swap(out);
				return true;
			}
		} else {
			if(this->tasks.empty()) {
				++this->watting;
				this->cv.wait(this->mtx);
				--this->watting;
			} else {
				this->tasks.swap(out);
				return true;
			}	
		}
	}
}

DbTask::Ptr DbThreadPool::TaskQueue::Get() {
	std::lock_guard<std::mutex> guard(this->mtx);
	for( ; ;) {
		if(this->closed) {
			if(this->tasks.empty()) {
				return nullptr;
			} else {
				DbTask::Ptr &task = this->tasks.front();
				this->tasks.pop_front();
				return task;
			}
		} else {

			if(this->tasks.empty()) {
				++this->watting;
				this->cv.wait(this->mtx);
				--this->watting;	
			} else {
				DbTask::Ptr task = this->tasks.front();
				this->tasks.pop_front();
				return task;
			}	
		}
	}
}


void DbThreadPool::TaskQueue::PostTask(const DbTask::Ptr &task) {
	std::lock_guard<std::mutex> guard(this->mtx);
	if(this->closed) {
		return;
	}	
	this->tasks.push_back(task);
	if(this->watting > 0) {
		this->cv.notify_one();
	}
}

bool DbThreadPool::Init(int threadCount) {

	bool expected = false;
	if(threadCount <= 0) {
		threadCount = 1;
	}

	for(auto i = 0; i < threadCount; ++i) {
		threads_.push_back(std::thread(threadFunc,&queue_));
	}

	return true;
}

DbThreadPool::~DbThreadPool() {
	queue_.Close();
	size_t i = 0;
	for( ; i < threads_.size(); ++i) {
		threads_[i].join();
	}	
}

