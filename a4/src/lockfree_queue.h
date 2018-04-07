#include <iostream>
#include <atomic>

template<typename T>
class msqueue {
private:
    struct node;

    struct ptr {
        node *p;
        unsigned int count;

        ptr() noexcept : p(nullptr), count(0) {}

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

    std::atomic<ptr> head;
    std::atomic<ptr> tail;
public:
    msqueue() : head(new node()), tail(head.load()) {}

    void enqueue(T value) {
        node *w = new node(value);
        // TODO fence(W||W)
        ptr t, n;
        while (true) {
            t = tail.load();
            n = t.p->next.load();
            if (t == tail.load()) {
                if (!n.p) {
                    if (t.p->next.compare_exchange_weak(n, ptr(w, n.count + 1))) {
                        break;
                    }
                } else {
                    tail.compare_exchange_weak(t, ptr(n.p, t.count + 1));
                }
            }
        }
        tail.compare_exchange_weak(t, ptr(w, t.count + 1));
    }

    T dequeue() {
        ptr h, t, n;
        T rtn;

        while (true) {
            h = head.load();
            t = tail.load();
            n = h.p->next.load();
            if (h == head.load()) {
                if (h.p == t.p) {
                    if (!n.p) {
                        // TODO maybe throw an exception
                        return NULL;
                    }
                    tail.compare_exchange_weak(t, ptr(n.p, t.count + 1));
                } else {
                    // read value before CAS; otherwise another dequeue might free n
                    rtn = n.p->val;
                    if (head.compare_exchange_weak(h, ptr(n.p, h.count + 1))) {
                        break;
                    }
                }
            }
        }

        // fence(W||W)
        // TODO free_for_reuse
        return rtn;
    }

    // non-concurrent call
    void dump(int cnt) {
        ptr curr = head;
        for (int i = 0; i <= cnt; i++) {
            std::cout << curr.p->val << "\n";
            curr = curr.p->next;
        }
    }
};
