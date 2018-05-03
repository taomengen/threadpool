#ifndef THREADPOLL_H
#define THREADPOLL_H

#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#include "syncqueue.h"

const int MaxTaskCount = 100;

class ThreadPool
{
public:
	using Task = std::function<void()>;
	ThreadPool(int numThreads = std::thread::hardware_concurrency())
		: m_queue(MaxTaskCount)
	{
		Start(numThreads);
	}
	~ThreadPool(void)
	{
		Stop();
	}
	void Stop();
	void AddTask(Task&&task);
	void AddTask(const Task &task);
private:
	void Start(int numThreads);
	void RunInThread();
	void StopThreadGroup();

private:
	std::list<std::shared_ptr<std::thread>> m_threadgroup; //处理任务的线程数
	SyncQueue<Task> m_queue;  //同步队列
	atomic_bool m_running;    //是否停止的标志
	std::once_flag m_flag;
};

#endif