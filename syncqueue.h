#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <iostream>

//类模板不能将声明和实现分离在两个文件中
template<typename T>
class SyncQueue
{
public:
	SyncQueue(int maxSize) : m_maxSize(maxSize), m_stop(false)
	{
		std::cout << "SyncQueue Construction" << std::endl;
	}
	~SyncQueue()
	{
		std::cout << "SyncQueue Destruction" << std::endl;
	}

	void Put(const T&x)
	{
		Add(x);
	}
	void Put(T&&x)
	{
		Add(std::forward<T>(x));
	}

	template<typename F>
	void Add(F&&x)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notFull.wait(locker, [this](){
			return m_stop || NotFull();
		});
		
		if (m_stop)
		{
			return;
		}
		m_list.push_back(std::forward<F>(x));
		m_notEmpty.notify_one();
	}

	//取任务，每次将list中的所有任务全部取出
	void Get(std::list<T> &list)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this](){
			return m_stop || NotEmpty();
		});

		if (m_stop)
		{
			return;
		}
		list = std::move(m_list);
		m_notFull.notify_one();
	}

	//取任务，每次将list中头部的一个任务取出
	void Get(T&t)
	{
		std::unique_lock<std::mutex> locker(m_mutex);
		m_notEmpty.wait(locker, [this](){
			return m_stop || NotEmpty();
		});

		if (m_stop)
		{
			return;
		}
		t = m_list.front();
		m_list.pop_front();
		m_notFull.notify_one();
	}
	
	bool Empty()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_list.empty();
	}
	bool Full()
	{
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_list.size() == m_maxSize;
	}

	bool NotEmpty() const
	{
		bool empty = m_list.empty();
		if (empty)
		{
			std::cout << "缓冲区空了，需要等待" << std::endl;
		}
		return !empty;
	}

	bool NotFull() const
	{
		bool full = m_list.size() >= m_maxSize;
		if (full)
		{
			std::cout << "缓冲区满了，需要等待" << std::endl;
		}
		return !full;
	}

	void StopList()
	{
		{
			std::lock_guard<std::mutex> locker(m_mutex);
			//同步队列停止
			m_stop = true;
		}
		m_notFull.notify_all();
		m_notEmpty.notify_all();
	}
	

private:

	std::list<T>            m_list;      // 缓冲区

	// 互斥量和条件变量结合起来使用
	std::mutex              m_mutex;     //　互斥量  
	std::condition_variable m_notEmpty;  // 不为空的条件变量
	std::condition_variable m_notFull;   // 不为满的条件变量

	int   m_maxSize;   // 同步队列的最大值
	bool  m_stop;      // 同步队列是否停止
};

#endif