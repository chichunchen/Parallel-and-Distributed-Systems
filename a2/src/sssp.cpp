/* -*- mode: c++ -*- */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "simplegraph_atomic.h"
#include "Timer.h"

#include <thread>
#include <string>
#include <atomic>

using namespace std;

const int INF = INT_MAX;
int threadNum = 1;

// init node as inf if it's not the src
void sssp_init(SimpleCSRGraphUII g, unsigned int src, int tid) {
	int total_nodes = g.num_nodes;
	int slice = total_nodes / threadNum;
	int start = tid * slice;
	int end = start + slice;
	if (tid+1 == threadNum) end = total_nodes;

	for (int i = start; i < end; i++) {
 		g.node_wt[i] = (i == src) ? 0 : INF;
	}
}

// accessed by sssp_round and main function
bool changed = false;

void sssp_round(SimpleCSRGraphUII g, int tid) {
	int total_nodes = g.num_nodes;
	int slice = total_nodes / threadNum;
	int start = tid * slice;
	int end = start + slice;
	if (tid+1 == threadNum) end = total_nodes;

	for(unsigned int node = start; node < end; node++) {
		if(g.node_wt[node] == INF) continue;

		for(unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {
			unsigned int dest = g.edge_dst[e];

 			while (true) {
 				int distance = g.node_wt[node].load() + g.edge_wt[e].load();
 				int prev_distance = g.node_wt[dest].load();
 
 				bool pred_1 = distance == (g.node_wt[node].load() + g.edge_wt[e]);
 				bool pred_2 = prev_distance == g.node_wt[dest].load();
 
 				// check whether distance and prev_distance has been successfully loaded
 				if (pred_1 && pred_2) {
 					if(prev_distance > distance) {
 						g.node_wt[dest].exchange(distance);
 						// check if atomic swap success
 						// if not success, then we continue to next loop
 						if (g.node_wt[dest].load() == distance) {
 							changed = true;
 							break;
 						}
 						else
 							continue;
 					}
 					break;
 				}
 			}
		}
	}
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
			r = fprintf(fp, "%d %d\n", i, g.node_wt[i].load());
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

	int src = 0, rounds = 0;
	threadNum = stoi(argv[3]);

	thread thread_arr[threadNum];
	t.start();

	// start of parallel sssp_init
	for (int i = 0; i < threadNum; ++i) {
		thread_arr[i] = thread(sssp_init, input, src, i);
	}
	for (int i = 0; i < threadNum; ++i) {
		thread_arr[i].join();
	}
	// end of parallel sssp_init

	// start of parallel sssp_round
	for(rounds = 0; rounds < input.num_nodes - 1; rounds++) {
		thread thread_arr[threadNum];
		changed = false;
		for (int i = 0; i < threadNum; ++i) {
			// side effect of sssp_round is to modify changed
			thread_arr[i] = thread(sssp_round, input, i);
		}
		for (int i = 0; i < threadNum; ++i) {
			thread_arr[i].join();
		}
		if(changed == false) {
			//no changes in graph, so exit early
			break;
		}
	}
	// end of parallel sssp_sssp

	t.stop();

	printf("%d rounds\n", rounds); /* parallel versions may have a different number of rounds */
	printf("Total time: %u ms\n", t.duration_ms());

	write_output(input, argv[2]);

	printf("Wrote output '%s'\n", argv[2]);
	return 0;
}
