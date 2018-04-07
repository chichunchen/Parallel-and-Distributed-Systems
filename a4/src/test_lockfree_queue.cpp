#include <iostream>
#include <thread>
#include "lockfree_queue.h"
#include "Timer.h"

#define THREAD_NUM 100
#define NUM 10000

void test_enqueue(msqueue<int> *queue, int tid) {
    int start = tid * NUM;
    int end = start + NUM;
    for (int i = start; i < end; i++) {
        queue->enqueue(i);
    }
}

void test_dequeue(msqueue<int> *queue) {
    for (int i = 0; i < NUM; i++) {
        queue->dequeue();
    }
}

void test_msqueue() {
    std::thread thread_arr[THREAD_NUM];

    auto *queue = new msqueue<int>;

    for (int i = 0; i < THREAD_NUM; i++) {
        thread_arr[i] = std::thread(test_enqueue, queue, i);
    }
    for (auto &i : thread_arr) {
        i.join();
    }
    for (int i = 0; i < THREAD_NUM; i++) {
        thread_arr[i] = std::thread(test_dequeue, queue);
    }
    for (auto &i : thread_arr) {
        i.join();
    }
}

int main() {
	ggc::Timer t("test_blocking");
	t.start();
    test_msqueue();
	t.stop();
	printf("Total time: %u ms\n", t.duration_ms());
}
