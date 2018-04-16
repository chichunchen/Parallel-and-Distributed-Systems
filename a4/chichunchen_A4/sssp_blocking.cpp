/* -*- mode: c++ -*- */

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "blocking_queue.h"
#include "Timer.h"

#include "atomicgraph.h"

#include <atomic>
#include <thread>
#include <string>
#include <vector>
#include <mutex>

int threadNum = 1;
const int INF = INT_MAX;

enum State {
	INIT,
	DO_WORK,
	NO_WORK,
};

void sssp_init(SimpleCSRGraphUII g, unsigned int src) {
    for (int i = 0; i < g.num_nodes; i++) {
        g.node_wt[i] = (i == src) ? 0 : INF;
    }
}

bool check_all_done(std::atomic<int> *states) {
    for (int i = 0; i < threadNum; i++) {
        if (states[i].load() != NO_WORK) {
            return false;
        }
    }
    return true;
}

std::atomic<int> counter;

void sssp(SimpleCSRGraphUII g, BlockingQueue *q, std::atomic<int> *states, int tid) {
    int node;

    while (true) {
        while (q->pop(node)) {
			// both INIT and NO_WORK change to DO_WORK
			states[tid].store(DO_WORK);

            for (unsigned int e = g.row_start[node]; e < g.row_start[node + 1]; e++) {
				while (true) {
					unsigned int dest = g.edge_dst[e];
					int distance = g.node_wt[node].load() + g.edge_wt[e];
					int prev_distance = g.node_wt[dest].load();
					bool pred_1 = distance == (g.node_wt[node].load() + g.edge_wt[e]);
					bool pred_2 = prev_distance == g.node_wt[dest].load();

					if (pred_1 && pred_2) {
						if (prev_distance > distance) {
							if (g.node_wt[dest].compare_exchange_weak(prev_distance, distance)) {
								// check if atomic swap success
								// if not success, then we continue to next loop
								if (!q->push(dest)) {
									fprintf(stderr, "ERROR: Out of queue space.\n");
									exit(1);
								}
								break;
							}
						} else {
							break;
						}
					}
				}
            }
        }
		int i;
		for (i = 0; i < threadNum; i++) {
			if (states[i].load() != NO_WORK) {
				break;
			}
		}
		if (i == threadNum)
			break;
		states[i].store(NO_WORK);
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

	ggc::Timer t("sssp");

    int src = 0;

    /* no need to parallelize this */
    sssp_init(input, src);

	t.start();
    auto *states = new std::atomic<int>[threadNum];
    for (int j = 0; j < threadNum; ++j) {
		states[j].store(INIT);
    }
	counter.store(0);

    bq.push(src);
    for (int i = 0; i < threadNum; i++) {
        thread_arr[i] = std::thread(sssp, input, &bq, states, i);
    }
    for (int i = 0; i < threadNum; i++) {
        thread_arr[i].join();
    }

	t.stop();

	printf("Total time: %u ms\n", t.duration_ms());

    write_output(input, argv[2]);

    printf("Wrote output '%s'\n", argv[2]);
    return 0;
}


