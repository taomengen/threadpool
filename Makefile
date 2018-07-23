test:test.cpp threadpool.cpp
	g++ -o test -std=c++11 test.cpp threadpool.cpp -g -pthread
clean:
	rm test	