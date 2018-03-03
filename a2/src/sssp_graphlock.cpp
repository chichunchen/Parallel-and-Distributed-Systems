/* -*- mode: c++ -*- */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "simplegraph.h"
#include "Timer.h"

const int INF = INT_MAX;

void sssp_init(SimpleCSRGraphUII g, unsigned int src) {
	for(int i = 0; i < g.num_nodes; i++) {
		g.node_wt[i] = (i == src) ? 0 : INF;
	}
}

bool sssp_round(SimpleCSRGraphUII g) {
	bool changed = false;

	for(unsigned int node = 0; node < g.num_nodes; node++) {
		if(g.node_wt[node] == INF) continue;

		for(unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {

			unsigned int dest = g.edge_dst[e];
			int distance = g.node_wt[node] + g.edge_wt[e];

			int prev_distance = g.node_wt[dest];

			if(prev_distance > distance) {
				g.node_wt[dest] = distance;
				changed = true;
			}
		}
	}

	return changed;
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
	if(argc != 3) {
		fprintf(stderr, "Usage: %s inputgraph outputfile\n", argv[0]);
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

	t.start();
	sssp_init(input, src);
	for(rounds = 0; rounds < input.num_nodes - 1; rounds++) {
		if(!sssp_round(input)) {
			//no changes in graph, so exit early
			break;
		}
	}
	t.stop();

	printf("%d rounds\n", rounds); /* parallel versions may have a different number of rounds */
	printf("Total time: %u ms\n", t.duration_ms());

	write_output(input, argv[2]);

	printf("Wrote output '%s'\n", argv[2]);
	return 0;
}
