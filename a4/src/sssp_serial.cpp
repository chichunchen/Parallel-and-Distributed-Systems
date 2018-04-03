/* -*- mode: c++ -*- */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "simplegraph.h"
#include "serialqueue.h"
#include "Timer.h"

const int INF = INT_MAX;

void sssp_init(SimpleCSRGraphUII g, unsigned int src) {
  for(int i = 0; i < g.num_nodes; i++) {
    g.node_wt[i] = (i == src) ? 0 : INF;
  }
}

bool sssp(SimpleCSRGraphUII g, SerialQueue *q) {
  bool changed = false;
  int node;

  while(q->pop(node)) {
    for(unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {

      unsigned int dest = g.edge_dst[e];
      int distance = g.node_wt[node] + g.edge_wt[e];

      int prev_distance = g.node_wt[dest];
      
      if(prev_distance > distance) {
	g.node_wt[dest] = distance;
	if(!q->push(dest)) {
	  fprintf(stderr, "ERROR: Out of queue space.\n");
	  exit(1);
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
  SerialQueue sq;

  if(!input.load_file(argv[1])) {
    fprintf(stderr, "ERROR: failed to load graph\n");
    exit(1);
  } 

  printf("Loaded '%s', %u nodes, %u edges\n", argv[1], input.num_nodes, input.num_edges);

  /* if you want to use dynamic allocation, go ahead */
  sq.initialize(input.num_edges * 2); // should be enough ...
  
  ggc::Timer t("sssp");

  int src = 0;

  /* no need to parallelize this */
  sssp_init(input, src);

  t.start();
  sq.push(src);
  sssp(input, &sq);
  t.stop();

  printf("Total time: %u ms\n", t.duration_ms());

  write_output(input, argv[2]);

  printf("Wrote output '%s'\n", argv[2]);
  return 0;
}
