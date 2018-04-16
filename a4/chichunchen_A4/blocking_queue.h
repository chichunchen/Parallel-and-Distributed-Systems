#include <stdlib.h>
#include <mutex>

std::mutex q_mutex;

class BlockingQueue {
	private:
		unsigned int qsize;
		unsigned int head;
		unsigned int tail;
		int *items;

	public:

		int initialize(unsigned int maxqsize) {
			qsize = maxqsize+1;
			items = (int *) calloc(qsize, sizeof(int));
			head = 0;
			tail = 0;
		}

		void dump() {
			int i = (head + 1) % qsize;
			int e = (tail + 1) % qsize;

			while(i != e) {
				printf("Q%d: %d\n", i, items[i]);
				i = (i + 1) % qsize;
			}
		}

		int push(int item) {
			q_mutex.lock();
			if((tail + 1) % qsize == head) { // queue is full
				q_mutex.unlock();
				return 0;
			}

			tail = (tail + 1) % qsize;
			items[tail] = item;

			q_mutex.unlock();
			return 1;
		}

		int pop(int &item) {
			q_mutex.lock();
			if(head == tail) {      		// queue is empty
				q_mutex.unlock();
				return 0;
			}

			head = (head + 1) % qsize;
			item = items[head];

			q_mutex.unlock();
			return 1;
		}  
};
