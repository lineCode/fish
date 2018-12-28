#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mutex>
#include <atomic>

class Task {
public:
	virtual void Do() = 0;	
	virtual ~Task() {}
};

class TaskQueue {
public:
	TaskQueue();
	virtual ~TaskQueue();

	void Push(Task* task) {
		std::lock_guard<std::mutex> guard(mutex_);
		std::queue<Task*>* queue = &queue_[writer_ % 2];
		queue->push(task);
	}

	Task* Pop() {
		std::queue<Task*>* queue = &queue_[reader_ % 2];
		if (!queue.empty()) {
			Task* task = queue->front();
			queue->pop();
			return task;
		}
		{
			std::lock_guard<std::mutex> guard(mutex_);
			reader_++;
			writer_++;
		}
		std::queue<Task*>* queue = &queue_[reader_ % 2];
		if (!queue.empty()) {
			Task* task = queue->front();
			queue->pop();
			return task;
		}
		return NULL;
	}

private:
	std::queue<Task*> queue_[2];
	std::mutex mutex_;
	std::atomic<int> reader_;
	std::atomic<int> writer_;
};

#endif