/*
 * blockingqueue.cpp
 *
 *  Created on: Dec 15, 2014
 *      Author: jonathanmeisel
 */

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename T>
class Queue
{
public:
	Queue(int max)
	: m_max{max}
	{}

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			emptycond_.wait(mlock);
		}
		auto item = std::move(queue_.front());
		queue_.pop();
		fullcond_.notify_one();
		return item;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			emptycond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
		fullcond_.notify_one();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.size() == m_max)
		{
			fullcond_.wait(mlock);
		}
		queue_.push(item);
		mlock.unlock();
		emptycond_.notify_one();
	}

	void push(T&& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.size() == m_max)
		{
			fullcond_.wait(mlock);
		}
		queue_.push(std::move(item));
		mlock.unlock();
		emptycond_.notify_one();
	}

	int size()
	{
		return queue_.size();
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable emptycond_;
	std::condition_variable fullcond_;
	int m_max;
};



