all: sssp_serial sssp sssp_no_atomic sssp_barrier sssp_graphlock sssp_nodelock sssp_relaxed sssp_ex3

sssp_serial: sssp_serial.cpp Timer.h simplegraph.h
	g++ -g -O3 $< -o $@

sssp: sssp.cpp Timer.h simplegraph_atomic.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

sssp_barrier: sssp_barrier.cpp Timer.h simplegraph_atomic.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

sssp_graphlock: sssp_graphlock.cpp Timer.h simplegraph.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

sssp_nodelock: sssp_nodelock.cpp Timer.h simplegraph.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

sssp_relaxed: sssp_relaxed.cpp Timer.h simplegraph_atomic.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

sssp_ex3: sssp_ex3.cpp Timer.h simplegraph.h
	g++ -g -std=c++11 -pthread -O3 $< -o $@

clean:
	rm sssp_serial sssp sssp_no_atomic sssp_graphlock sssp_nodelock sssp_relaxed sssp_barrier sssp_ex3
