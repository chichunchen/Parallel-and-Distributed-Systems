all: sssp_serial sssp sssp_graphlock sssp_nodelock sssp_relaxed

sssp_serial: sssp_serial.cpp Timer.h simplegraph.h
	g++ -g -O3 $< -o $@

sssp: sssp.cpp Timer.h simplegraph.h
	g++ -g -O3 $< -o $@

sssp_graphlock: sssp_graphlock.cpp Timer.h simplegraph.h
	g++ -g -O3 $< -o $@

sssp_nodelock: sssp_nodelock.cpp Timer.h simplegraph.h
	g++ -g -O3 $< -o $@

sssp_relaxed: sssp_relaxed.cpp Timer.h simplegraph.h
	g++ -g -O3 $< -o $@

clean:
	rm sssp_serial sssp sssp_graphlock sssp_nodelock sssp_relaxed
