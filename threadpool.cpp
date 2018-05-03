#include "threadpool.h"

void ThreadPool::Stop()
{
	//保证多线程情况下只调用一次StopThreadGroup
	std::call_once(m_flag, [this]{StopThreadGroup();});
}

void ThreadPool::AddTask(Task&&task)
{
	m_queue.Put(std::forward<Task>(task));
}

void ThreadPool::AddTask(const Task &task)
{
	m_queue.Put(task);
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
		m_queue.Take(list);

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

void ThreadPool::StopThreadGroup()
{
	m_queue.Stop(); //让同步队列中的线程停止
	m_running = false; //让内部线程跳出循环并退出

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