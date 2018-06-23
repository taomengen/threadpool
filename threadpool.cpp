#include "threadpool.h"

//参数默认值放在声明中，初始化列表放在实现中
ThreadPool::ThreadPool(int numThreads) : m_syncqueue(MaxTaskCount)
{
	cout << "numThreads = " << std::thread::hardware_concurrency() << endl;
	Start(numThreads);
}

ThreadPool::~ThreadPool()
{
	Stop();	
}

void ThreadPool::AddTask(Task &&task)
{
	m_syncqueue.Put(std::forward<Task>(task));
}

void ThreadPool::AddTask(const Task &task)
{
	m_syncqueue.Put(task);
}

void ThreadPool::Start(int numThreads)
{
	m_running = true;
	// 创建线程组
	for (int i = 0; i < numThreads; i++)
	{
		m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
	}
}

void ThreadPool::RunInThread()
{
	while (m_running)
	{
		//取任务分别执行
		std::list<Task> list;
		m_syncqueue.Get(list);

		cout << "m_syncqueue.Get(list) list.size() = " << list.size() << endl;

		for (auto &task: list)
		{
			if (! m_running)
			{
				return;
			}
			task();
		}
	}
}

void ThreadPool::Stop()
{
	//停止
	m_running = false;

	//保证多线程情况下只调用一次StopThreadGroup
	std::call_once(m_flag, [this]{StopThreadGroup();});
}

void ThreadPool::StopThreadGroup()
{
	//停止同步队列中的线程
	m_syncqueue.Stop();    
	
	// 等待线程结束
	for (auto thread : m_threadgroup)
	{
		if (thread)
		{
			thread->join();
		}
		m_threadgroup.clear();
	}
}