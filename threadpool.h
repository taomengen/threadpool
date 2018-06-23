#ifndef THREADPOLL_H
#define THREADPOLL_H

#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#include "syncqueue.h"

const int MaxTaskCount = 10;

class ThreadPool
{
public:
	using Task = std::function<void()>;

	//参数默认值放在声明中，初始化列表放在实现中
	ThreadPool(int numThreads = std::thread::hardware_concurrency());
	~ThreadPool(void);
	
	void AddTask(Task&&task);
	void AddTask(const Task &task);
	void Start(int numThreads);
	void RunInThread();
	void Stop();
	void StopThreadGroup();

private:

	//容器对象
	std::list<std::shared_ptr<std::thread>>  m_threadgroup; //线程组

	//模板类对象
	SyncQueue<Task>    m_syncqueue;  //同步队列

	std::atomic_bool   m_running;    //是否停止的标志

	std::once_flag     m_flag;
};

#endif