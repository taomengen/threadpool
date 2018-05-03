test:test.cpp syncqueue.h threadpool.h threadpool.cpp
	g++ -o test -std=c++11 test.cpp syncqueue.h threadpool.h threadpool.cpp -g -lpthread
clean:
	rm test	