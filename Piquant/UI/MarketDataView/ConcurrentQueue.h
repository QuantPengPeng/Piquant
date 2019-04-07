#ifndef _CONCURRENT_QUEUE_H
#define _CONCURRENT_QUEUE_H

#include <queue>
#include <deque>
#include <mutex>
#include <condition_variable>

/*消息队列实现，基于C++11实现*/
template<class Type>
class ConcurrentQueue 
{
	ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
	ConcurrentQueue(const ConcurrentQueue& other) = delete;

public:
	ConcurrentQueue() : _queue(), _mutex(), _condition() { }
	virtual ~ConcurrentQueue() { }

	void Push(Type record) 
	{
		std::lock_guard <std::mutex> lock(_mutex);
		_queue.push_back(record);
		_condition.notify_one();
	}

	bool Pop(Type& record, bool isBlocked = true) 
	{
		/*
		if (isBlocked)
		{
		std::unique_lock <std::mutex> lock(_mutex);
		while (_queue.empty())
		{
		_condition.wait(lock);
		}
		}
		else // If user wants to retrieve data in non-blocking mode
		{
		std::lock_guard <std::mutex> lock(_mutex);
		if (_queue.empty())
		{
		return false;
		}
		}
		record = std::move(_queue.front());//先进先出
		_queue.pop();
		return true;
		*/
		std::unique_lock<std::mutex> lock(_mutex);
		_condition.wait(lock, [this]{  return !this->_queue.empty(); });
		assert(!_queue.empty());
		record=std::move(_queue.front());
		_queue.pop_front();
		return  TRUE;
	}

	int32_t Size() 
	{
		std::lock_guard <std::mutex> lock(_mutex);
		return _queue.size();
	}

	bool Empty() 
	{
		std::lock_guard <std::mutex> lock(_mutex);
		return _queue.empty();
	}

private:
	std::deque<Type> _queue;
	mutable std::mutex _mutex;
	std::condition_variable _condition;
};
#endif //_CONCURRENT_QUEUE_H
