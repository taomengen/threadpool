#include "threadpool.h"
#include "syncqueue.h"
#include <sys/time.h>

int main(int argc, char* argv[])
{
	ThreadPool threadpool;

	std::thread t1([&threadpool](){
		for (int i = 0; i < 20; i++)
		{
			auto id = std::this_thread::get_id();
			threadpool.AddTask([id](){
				std::cout << "同步线程1的线程ID:" << id << std::endl;	
		    });
		}
	});

	std::thread t2([&threadpool](){
		for (int i = 0; i < 20; i++)
		{
			auto id = std::this_thread::get_id();
			threadpool.AddTask([id](){
				std::cout << "同步线程2的线程ID:" << id << std::endl;
			});
		}
	});


	getchar();

	t1.join();
	t2.join();

	return 0;
}