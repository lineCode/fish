#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mutex>
#include <atomic>
#include <queue>

class MainTask {
public:
	virtual void MainDo() = 0;	
	virtual ~MainTask() {}
};

class TaskQueue {
public:
	TaskQueue() : reader_(0), writer_(1) {

	}

	virtual ~TaskQueue() {

	}

	void Prepare() {
		std::lock_guard<std::mutex> guard(mutex_);
		reader_++;
		writer_++;
	}

	void Push(MainTask* task) {
		std::lock_guard<std::mutex> guard(mutex_);
		std::queue<MainTask*>* queue = &queue_[writer_ % 2];
		queue->push(task);
	}

	MainTask* Pop() {
		std::queue<MainTask*>* queue = &queue_[reader_ % 2];
		if (!queue->empty()) {
			MainTask* task = queue->front();
			queue->pop();
			return task;
		}
		return NULL;
	}

private:
	std::queue<MainTask*> queue_[2];
	std::mutex mutex_;
	uint64_t reader_;
	uint64_t writer_;
};

#endif