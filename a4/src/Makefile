all: sssp_blocking sssp_lockfree

sssp_blocking: sssp_blocking.cpp Timer.h atomicgraph.h blocking_queue.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

sssp_lockfree: sssp_lockfree.cpp Timer.h atomicgraph.h lockfree_queue.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@ -latomic

clean:
	rm sssp_blocking sssp_lockfree
