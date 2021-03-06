/* -*- mode: c++ -*- */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "simplegraph.h"
#include "Timer.h"

#include <thread>
#include <string>
#include <pthread.h>

using namespace std;

const int INF = INT_MAX;
int threadNum = 1;

bool changed = false;
pthread_barrier_t mybarrier;

void sssp(SimpleCSRGraphUII g, int tid, int* rounds_ptr) {
	int total_nodes = g.num_nodes;
	int slice = total_nodes / threadNum;
	int start = tid * slice;
	int end = start + slice;
	if (tid+1 == threadNum) end = total_nodes;

	int rounds;
	int src = 0;

	string timer_name = "thread-" + to_string(tid);
	long long duration = 0;
	ggc::Timer t(timer_name.c_str());

	t.start();
	for (int i = start; i < end; i++) {
 		g.node_wt[i] = (i == src) ? 0 : INF;
	}
	t.stop();
	duration += t.duration();

	for(rounds = 0; rounds < total_nodes - 1; rounds++) {
		t.start();
		for(unsigned int node = start; node < end; node++) {
			if(g.node_wt[node] == INF) continue;

			for(unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {

				unsigned int dest = g.edge_dst[e];
				int distance = g.node_wt[node] + g.edge_wt[e];

				int prev_distance = g.node_wt[dest];

				if(prev_distance > distance) {
					g.node_wt[dest] = distance;
					//printf("tt %d %d\n", tid, g.node_wt[dest]);
					changed = true;
				}
			}
		}
		t.stop();
		duration += t.duration();

		pthread_barrier_wait(&mybarrier);
		if(changed == false) break;
		pthread_barrier_wait(&mybarrier);
		changed = false;
		pthread_barrier_wait(&mybarrier);
	}

	int duration_ms = duration * 1000 / NANOSEC;  
	printf("[%s] running time: %u ms\n", timer_name.c_str(), duration_ms);

	*rounds_ptr = rounds;
}

void write_output(SimpleCSRGraphUII &g, const char *out) {
	FILE *fp; 

	fp = fopen(out, "w");
	if(!fp) {
		fprintf(stderr, "ERROR: Unable to open output file '%s'\n", out);
		exit(1);
	}

	for(int i = 0; i < g.num_nodes; i++) {
		int r;
		if(g.node_wt[i] == INF) {
			r = fprintf(fp, "%d INF\n", i);
		} else {
			r = fprintf(fp, "%d %d\n", i, g.node_wt[i]);
		}

		if(r < 0) {
			fprintf(stderr, "ERROR: Unable to write output\n");
			exit(1);
		}
	}
}

int main(int argc, char *argv[]) 
{
	if(argc != 4) {
		fprintf(stderr, "Usage: %s inputgraph outputfile numberofthreads\n", argv[0]);
		exit(1);
	}

	SimpleCSRGraphUII input;

	if(!input.load_file(argv[1])) {
		fprintf(stderr, "ERROR: failed to load graph\n");
		exit(1);
	} 

	printf("Loaded '%s', %u nodes, %u edges\n", argv[1], input.num_nodes, input.num_edges);

	ggc::Timer t("sssp");

	int src = 0;
	threadNum = stoi(argv[3]);

	thread thread_arr[threadNum];
	pthread_barrier_init(&mybarrier, NULL, threadNum);

	t.start();

	// start of parallel sssp
	int rounds = 0;
	int* rounds_ptr = &rounds;
	for (int i = 0; i < threadNum; ++i) {
		thread_arr[i] = thread(sssp, input, i, rounds_ptr);
	}
	for (int i = 0; i < threadNum; ++i) {
		thread_arr[i].join();
	}
	// end of parallel sssp_sssp

	t.stop();

	pthread_barrier_destroy(&mybarrier);

	printf("%d rounds\n", rounds); /* parallel versions may have a different number of rounds */
	printf("Total time: %u ms\n", t.duration_ms());

	write_output(input, argv[2]);

	printf("Wrote output '%s'\n", argv[2]);
	return 0;
}
