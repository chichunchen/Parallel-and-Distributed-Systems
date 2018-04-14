/* -*- mode: c++ -*- */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "blocking_queue.h"
//#include "Timer.h"

#include "atomicgraph.h"

#include <atomic>
#include <thread>
#include <string>
#include <vector>

int threadNum = 1;
const int INF = INT_MAX;

void sssp_init(SimpleCSRGraphUII g, unsigned int src) {
    for (int i = 0; i < g.num_nodes; i++) {
        g.node_wt[i] = (i == src) ? 0 : INF;
    }
}

bool check_all_done(std::atomic<bool> *done) {
    for (int i = 0; i < threadNum; i++) {
        if (!done[i].load()) {
            return false;
        }
    }
    return true;
}


// TODO has to guarantee that each thread use different node in for-loop and each thread push different dest into bq
// some quick thoughts:
// use another bq to store the
void sssp(SimpleCSRGraphUII g, BlockingQueue *q, std::atomic<bool> *done, int tid) {
    int node;

    while (!check_all_done(done)) {
        while (q->pop(node)) {
            done[tid].store(false);
//            printf("node: %d, thread: %d\n", node, tid);
            fflush(stdout);

            for (unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {

                unsigned int dest = g.edge_dst[e];
                int distance = g.node_wt[node] + g.edge_wt[e];

                int prev_distance = g.node_wt[dest];

                if (prev_distance > distance) {
                    g.node_wt[dest] = distance;

                    // skip the dest we have pushed
                    if (!q->push(dest)) {
                        fprintf(stderr, "ERROR: Out of queue space.\n");
                        exit(1);
                    }
                }
            }
        }
        done[tid].store(true);
    }
}

void write_output(SimpleCSRGraphUII &g, const char *out) {
    FILE *fp;

    fp = fopen(out, "w");
    if (!fp) {
        fprintf(stderr, "ERROR: Unable to open output file '%s'\n", out);
        exit(1);
    }

    for (int i = 0; i < g.num_nodes; i++) {
        int r;
        if (g.node_wt[i] == INF) {
            r = fprintf(fp, "%d INF\n", i);
        } else {
            r = fprintf(fp, "%d %d\n", i, g.node_wt[i].load());
        }

        if (r < 0) {
            fprintf(stderr, "ERROR: Unable to write output\n");
            exit(1);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s inputgraph outputfile\n", argv[0]);
        exit(1);
    }

    SimpleCSRGraphUII input;
    BlockingQueue bq;

    if (!input.load_file(argv[1])) {
        fprintf(stderr, "ERROR: failed to load graph\n");
        exit(1);
    }

    threadNum = std::stoi(argv[3]);
    std::thread thread_arr[threadNum];

    printf("Loaded '%s', %u nodes, %u edges\n", argv[1], input.num_nodes, input.num_edges);

    /* if you want to use dynamic allocation, go ahead */
    bq.initialize(input.num_edges * 2); // should be enough ...

//	ggc::Timer t("sssp");

    int src = 0;

    /* no need to parallelize this */
    sssp_init(input, src);

//	t.start();
    auto *done = new std::atomic<bool>[threadNum];
    for (int j = 0; j < threadNum; ++j) {
        done->store(false);
    }

    bq.push(src);
    for (int i = 0; i < threadNum; i++) {
        thread_arr[i] = std::thread(sssp, input, &bq, done, i);
    }
    for (int i = 0; i < threadNum; i++) {
        thread_arr[i].join();
    }

//	t.stop();

//	printf("Total time: %u ms\n", t.duration_ms());

    write_output(input, argv[2]);

    printf("Wrote output '%s'\n", argv[2]);
    return 0;
}

