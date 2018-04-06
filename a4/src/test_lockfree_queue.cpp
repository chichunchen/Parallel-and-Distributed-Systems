#include <iostream>
#include <thread>
#include "lockfree_queue.h"

void test_enqueue(msqueue<int> *queue, int tid) {
    int start = tid * 10;
    int end = start + 10;
    for (int i = start; i < end; i++) {
        queue->enqueue(i);
    }
}

void test_dequeue(msqueue<int> *queue) {
    for (int i = 0; i < 10; i++) {
        queue->dequeue();
    }
}

void test_msqueue() {
    const int n = 8;
    std::thread thread_arr[n];

    auto *queue = new msqueue<int>;

    for (int i = 0; i < n; i++) {
        thread_arr[i] = std::thread(test_enqueue, queue, i);
    }
    for (auto &i : thread_arr) {
        i.join();
    }
    for (int i = 0; i < n; i++) {
        thread_arr[i] = std::thread(test_dequeue, queue);
    }
    for (auto &i : thread_arr) {
        i.join();
    }
}

int main() {
    test_msqueue();
}
