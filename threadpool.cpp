#include "threadpool.h"

//参数默认值放在声明中，初始化列表放在实现中
ThreadPool::ThreadPool(int numThreads) : m_syncqueue(TaskMaxSize)
{
	std::cout << "numThreads = " << std::thread::hardware_concurrency() << std::endl;
	std::cout << "ThreadPool Construction" << std::endl;

	//保证多线程情况下只调用一次CreateThreadGroup
	std::call_once(m_flag, [this, numThreads](){
		CreateThreadGroup(numThreads);
	});	
}

void ThreadPool::CreateThreadGroup(int numThreads)
{
	//创建线程组
	for (int i = 0; i < numThreads; i++)
	{
		m_threadgroup.push_back(std::make_shared<std::thread>(&ThreadPool::Function, this));
	}

	//线程池活着
	m_threadpool_alive = true;
}

ThreadPool::~ThreadPool()
{
	std::cout << "ThreadPool Destruction" << std::endl;

	//保证多线程情况下只调用一次DestroyThreadGroup
	std::call_once(m_flag, [this](){
		DestroyThreadGroup();
	});	
}

void ThreadPool::DestroyThreadGroup()
{
	// 停止同步队列
	m_syncqueue.StopList();    
	
	// 销毁线程组，等待线程组中的线程结束
	for (auto &thread : m_threadgroup)
	{
		if (thread != NULL)
		{
			thread->join();
		}
		m_threadgroup.clear();
	}

	//线程池死了
	m_threadpool_alive = false;
}


void ThreadPool::AddTask(Task&&task)
{
	m_syncqueue.Put(std::forward<Task>(task));
}

void ThreadPool::AddTask(const Task&task)
{
	m_syncqueue.Put(task);
}

void ThreadPool::Function()
{
	//线程函数不断查询线程池是否活着，如果活着，从同步队列中取出任务执行
	while (m_threadpool_alive)
	{
		//线程函数每次取出list中的所有任务
		std::list<Task> list;
		m_syncqueue.Get(list);

		std::cout << "list.size() = " << list.size() << std::endl;

		//线程函数执行任务
		for (auto &task : list)
		{
			//在执行任务之前进行判断
			//如果线程池活着，执行任务
			//如果线程池死了，就不再执行任务了，直接返回
			if (!m_threadpool_alive)
			{
				return;
			}
			//执行任务
			task();
		}
	}
}

