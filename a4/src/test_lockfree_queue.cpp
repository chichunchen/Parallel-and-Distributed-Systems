#include <iostream>
#include <thread>
#include "lockfree_queue.h"

#define THREAD_NUM	16
#define NODE_NUM	1000

void test_enqueue(msqueue<int> *queue, int tid) {
    int start = tid * NODE_NUM;
    int end = start + NODE_NUM;
    for (int i = start; i < end; i++) {
        queue->enqueue(i);
    }
}

void test_dequeue(msqueue<int> *queue) {
	int node;
	while (queue->dequeue(node));
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
//     for (int i = 0; i < THREAD_NUM; i++) {
//         thread_arr[i] = std::thread(test_dequeue, queue);
//     }
//     for (auto &i : thread_arr) {
//         i.join();
//     }
	queue->dump(THREAD_NUM * NODE_NUM);
}

int main() {
    test_msqueue();
}
