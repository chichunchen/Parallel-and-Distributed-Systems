all: sssp_serial test_blocking sssp_blocking test_lockfree_queue

sssp_serial: sssp_serial.cpp Timer.h simplegraph.h serialqueue.h
	g++ -g -O3 $< -o $@

test_blocking: test_blocking.cpp blocking_queue.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

test_lockfree_queue: test_lockfree_queue.cpp lockfree_queue.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@ -latomic

sssp_blocking: sssp_blocking.cpp Timer.h simplegraph.h blocking_queue.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

clean:
	rm sssp_serial sssp_blocking test_lockfree_queue test_blocking
