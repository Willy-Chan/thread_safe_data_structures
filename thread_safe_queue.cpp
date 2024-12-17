/*
 * This is the most basic implementation of a thread-safe queue possible.
 *
 * Essentially, there is one, huge global lock that protects the entire queue data structure,
 * and serializes accesses by different threads.
 * */

#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class threadsafe_queue {
	private:
		queue<int> my_global_queue;
		mutex single_mtx;
	public:
		void push(int new_value) {
			unique_lock<mutex> lock(single_mtx);
			my_global_queue.push(new_value);

			return;
		}

		int pop() {
			unique_lock<mutex> lock(single_mtx);
			int popped_val = my_global_queue.front();
			my_global_queue.pop();    // std queue has separate top and pop, my queue makes it so that you just get the top
			return popped_val;
		}

		bool empty() {
			unique_lock<mutex> lock(single_mtx);
			
			return my_global_queue.empty();
		}
};



void thread_1_operations(threadsafe_queue* q) {
	q->push(0);	
	q->push(10);
	q->push(20);
	q->push(30);
}


void thread_2_operations(threadsafe_queue* q) {
	int val = q->pop();
	cout << "popped value " << val << " from queue." << endl;	
}

int main(int argc, char* argv[]) {
    	int num_threads_to_spawn = 10;	


	threadsafe_queue q;
	
	thread* arr_of_threads = new thread[num_threads_to_spawn];

	for (int i = 0; i < num_threads_to_spawn; i++) {
		if (i % 2 == 0) {
			arr_of_threads[i] = thread(thread_2_operations, &q); 
		} else if (i % 2 == 1) {
			arr_of_threads[i] = thread(thread_1_operations, &q); 
		}
	}	


	// thread worker_1(thread_1_operations, &q);
	// thread worker_2(thread_2_operations, &q);	

	
	// worker_1.join();
	// worker_2.join();
	//
	
	for (int i = 0; i < num_threads_to_spawn; i++) {
		arr_of_threads[i].join();
	}

	while (!q.empty()) {
		cout << q.pop() << "  ";
	}
	cout << endl;

	return 0;
}
