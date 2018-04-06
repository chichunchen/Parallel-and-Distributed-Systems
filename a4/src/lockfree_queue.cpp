#include <iostream>
#include <atomic>
#include <thread>

// TODO break on cycle ...

template<typename T>
class msqueue {
private:
    struct node;

    struct alignas(16) ptr {
        node *p;
        unsigned int count;

        ptr() : p(nullptr), count(0) {}

        ptr(node *ptr) : p(ptr), count(0) {}

        ptr(node *ptr, unsigned int count) : p(ptr), count(count) {}

        bool operator==(const ptr &other) const {
            return p == other.p && count == other.count;
        }
    };

    struct node {
        T val;
        std::atomic<ptr> next;

        // dummy node
        node() : next(ptr()) {}

        // normal node
        node(T value) : val(value), next(ptr()) {}
    };

    alignas(16) std::atomic<ptr> head;
    alignas(16) std::atomic<ptr> tail;
public:
	msqueue() : head{new node{}}, tail{head.load()} {}

     void enqueue(T value) {
         node *w = new node(value);
         // implicitly fence(W||W) since x86 has sequentially consistency as default
         ptr t, n;
         while (1) {
             t = tail.load();
             n = t.p->next.load();
             if (t == tail.load()) {
                 if (!n.p) {
                     if (t.p->next.compare_exchange_strong(n, ptr(w, n.count + 1))) {
                         break;
                     }
                 } else {
                     tail.compare_exchange_strong(t, ptr(n.p, t.count + 1));
                 }
             }
         }
         tail.compare_exchange_strong(t, ptr(w, t.count + 1));
     }
 
     // non-concurrent
     void dump(int cnt) {
         ptr curr = head;
         for (int i = 0; i <= cnt; i++) {
             std::cout << curr.p->val << "\n";
             curr = curr.p->next;
         }
     }
};


void test_enqueue (msqueue<int> *queue, int tid) {
    int start = tid * 10;
    int end = start + 10;
    for (int i = start; i < end; i++) {
        queue->enqueue(i);
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
	std::cout << "wait a bit" << std::endl;
    queue->dump(80);
}

int main() {
    test_msqueue();
}
