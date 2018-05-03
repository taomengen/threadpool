#include "threadpool.h"
#include "syncqueue.h"

int main(int argc, char* argv[])
{
	ThreadPool pool;
	std::thread t1([&pool]{
		for (int i = 0; i < 10; i++)
		{
			auto id = this_thread::get_id();
			pool.AddTask([id]{
				cout << "同步线程1的线程ID:" << id << endl;
			});
		}
	});

	std::thread t2([&pool]{
		for (int i = 0; i < 10; i++)
		{
			auto id = this_thread::get_id();
			pool.AddTask([id]{
				cout << "同步线程2的线程ID:" << id << endl;
			});
		}
	});

	this_thread::sleep_for(std::chrono::seconds(2));
	getchar();
	pool.Stop();
	t1.join();
	t2.join();
}