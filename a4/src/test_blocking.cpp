#include <thread>
#include "blocking_queue.h"
#include "Timer.h"

#define THREAD_NUM	100
#define NODE_NUM	10000

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

	ggc::Timer t("test_blocking");

	BlockingQueue bq;
	bq.initialize(NODE_NUM * THREAD_NUM);

	t.start();

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

	t.stop();
	printf("Total time: %u ms\n", t.duration_ms());

	return 0;
}
