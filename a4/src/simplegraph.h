#pragma once
#include <stdio.h>
#include <stdlib.h>

template <typename index_type, typename wt_type>
class SimpleCSRGraph { 
 public:
  
  int num_edges;
  int num_nodes;
  index_type *row_start;
  index_type *edge_dst;
  wt_type *edge_wt;
  wt_type *node_wt;

  int load_file(const char *f) {
    FILE *fp;

    fp = fopen(f, "r");
    if(fp == NULL) {
      fprintf(stderr, "ERROR: Couldn't open file %s\n", f);
      return 0;
    }

    if(fscanf(fp, "%d %d", &num_nodes, &num_edges) == EOF) {
      fprintf(stderr, "ERROR: Couldn't read nodes and edges\n");
      fclose(fp);
      return 0;
    }
    
    unsigned int src, dst, wt, last_src = 0;
    int rd;

    edge_dst = (index_type *)  calloc(num_edges, sizeof(index_type));
    edge_wt = (wt_type *) calloc(num_edges, sizeof(wt_type));

    row_start = (index_type *) calloc(num_nodes + 1, sizeof(index_type));
    node_wt = (wt_type *) calloc(num_nodes, sizeof(wt_type));

    for(int i = 0; i < num_edges; i++) {
      rd = fscanf(fp, "%d %d %d", &src, &dst, &wt);

      if(rd == EOF || rd < 3) {
	fprintf(stderr, "ERROR: Couldn't read edge (%d)\n", i);
	fclose(fp);
	return 0;
      }

      if(last_src > src) {
	fprintf(stderr, "ERROR:%s:%d: Source nodes in file must be in ascending order\n", f, i);
	fclose(fp);
	return 0;
      }

      while (last_src != src) {
	row_start[++last_src] = i;
      }

      edge_dst[i] = dst;
      edge_wt[i] = wt;
    }

    while (last_src < num_nodes) {
	row_start[++last_src] = num_edges;
    }    

    fclose(fp);
    return 1;
  }

  void dump() {
    printf("row_start: ");
    for(int i = 0; i < num_nodes+1; i++) {
      printf("%u ", row_start[i]);
    }
    printf("\n");

    printf("edge_dst: ");
    for(int i = 0; i < num_edges; i++) {
      printf("%u ", edge_dst[i]);
    }    
    printf("\n");

    printf("edge_wt: ");
    for(int i = 0; i < num_edges; i++) {
      printf("%d ", edge_wt[i]);
    }    
    printf("\n");
  }

  int save_file(const char *f) {
    FILE *fp;

    fp = fopen(f, "w");
    if(!fp) {
      fprintf(stderr, "ERROR: Unable to open file '%s' for writing.\n", f);
      return 0;
    }

    if(fprintf(fp, "%d %d\n", num_nodes, num_edges) < 0) {
      fprintf(stderr, "ERROR: Unable to write header\n");
      fclose(fp);
      return 0;
    }

    for(int i = 0; i < num_nodes; i++) {
      for(int e = row_start[i]; e < row_start[i+1]; e++) {
	if(fprintf(fp, "%d %d %d\n", i, edge_dst[e], edge_wt[e]) < 0) {
	  fprintf(stderr, "ERROR: Unable to write edge data for edge %d\n", e);
	  fclose(fp);
	  return 0;
	}
      }
    }

    fclose(fp);
    return 1;
  }
};

typedef SimpleCSRGraph<unsigned int, int> SimpleCSRGraphUII;

