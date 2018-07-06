#ifndef THREADPOLL_H
#define THREADPOLL_H

#include <list>
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#include "syncqueue.h"

const int TaskMaxSize = 10;

class ThreadPool
{
public:
	using Task = std::function<void()>;

	//参数默认值放在声明中，初始化列表放在实现中
	ThreadPool(int numThreads = std::thread::hardware_concurrency());
	~ThreadPool(void);
	
	void AddTask(Task&&task);
	void AddTask(const Task &task);
	void Function();
	void StopThreadGroup();

private:

	//容器对象
	std::list<std::shared_ptr<std::thread>>  m_threadgroup; //线程组

	//模板类对象
	SyncQueue<Task>    m_syncqueue;          //同步队列

	std::atomic_bool   m_threadpool_alive;   //线程池是否活着

	std::once_flag     m_flag;               //只调用一次的标志

};

#endif