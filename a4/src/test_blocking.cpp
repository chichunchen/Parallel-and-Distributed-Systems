#include <thread>
#include "blocking_queue.h"

#define THREAD_NUM	8
#define NODE_NUM	100

using namespace std;

void test_push(BlockingQueue *bq, int tid) {
	for (int i = 0; i < NODE_NUM; i++) {
		bq->push(i);
	}
}

void test_pop(BlockingQueue *bq, int tid) {
	int node;
	for (int i = 0; i < NODE_NUM; i++) {
		bq->pop(node);
	}
}

int main() {
	thread thread_arr[THREAD_NUM];

	BlockingQueue bq;
	bq.initialize(NODE_NUM * THREAD_NUM);

	for (int i = 0; i < THREAD_NUM; i++) {
		thread_arr[i] = thread(test_push, &bq, i);
	}
	for (int i = 0; i < THREAD_NUM; ++i) {
		thread_arr[i].join();
	}
 	for (int i = 0; i < THREAD_NUM; i++) {
 		thread_arr[i] = thread(test_pop, &bq, i);
 	}
	for (int i = 0; i < THREAD_NUM; ++i) {
		thread_arr[i].join();
	}
	bq.dump();
	return 0;
}
