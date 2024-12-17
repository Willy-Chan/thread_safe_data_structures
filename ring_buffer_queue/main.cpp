#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <atomic>

template <typename T>
class RingBufferQueue {
public:
    RingBufferQueue(int capacity) : capacity(capacity), front(0), rear(0), count(0) {
        buffer = new T[capacity * 5];
    }

    ~RingBufferQueue() {
        delete[] buffer;
    }

    void enqueue(T item) {
	
        buffer[rear] = item;
        rear = (rear + 1) % capacity;
	
	if (front == rear) {
		throw std::overflow("FRONT TOUCHING REAR!!!")
	}

        ++count;
    }

    T dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("Queue is empty");
        }
        T item = buffer[front];
        front = (front + 1) % capacity;
        --count;
        return item;
    }

    bool isFull() const {
        return count == capacity;
    }

    bool isEmpty() const {
        return count == 0;
    }

    int size() const {
        return count;
    }

private:
    T* buffer;
    int front;
    int rear;
    int count;
    int capacity;
};

void testSingleThreadEnqueueDequeue() {
    RingBufferQueue<int> queue(5);

    // Single-threaded enqueue and dequeue
    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    assert(queue.size() == 3 && "Queue size should be 3 after enqueues");

    int item1 = queue.dequeue();
    assert(item1 == 1 && "Dequeued item should be 1");

    int item2 = queue.dequeue();
    assert(item2 == 2 && "Dequeued item should be 2");

    int item3 = queue.dequeue();
    assert(item3 == 3 && "Dequeued item should be 3");

    assert(queue.isEmpty() && "Queue should be empty after dequeuing all items");
}

void testMultiThreadedEnqueue() {
    RingBufferQueue<int> queue(5);
    std::atomic<int> counter(0);

    auto enqueueTask = [&queue, &counter]() {
        for (int i = 0; i < 5; ++i) {
            queue.enqueue(counter.fetch_add(1));
        }
    };

    std::thread t1(enqueueTask);
    std::thread t2(enqueueTask);

    t1.join();
    t2.join();

    assert(queue.size() == 10 && "Queue should have 10 items after two threads enqueue");
}

void testMultiThreadedDequeue() {
    RingBufferQueue<int> queue(5);

    // Enqueue 10 items first
    for (int i = 0; i < 10; ++i) {
        queue.enqueue(i);
    }

    std::vector<int> results;
    auto dequeueTask = [&queue, &results]() {
        while (!queue.isEmpty()) {
            results.push_back(queue.dequeue());
        }
    };

    std::thread t1(dequeueTask);
    std::thread t2(dequeueTask);

    t1.join();
    t2.join();

    assert(results.size() == 10 && "Total dequeued items should be 10");
    for (int i = 0; i < 10; ++i) {
        assert(results[i] == i && "Dequeued items should be in the correct order");
    }
}

void testConcurrencyWithWrapAround() {
    RingBufferQueue<int> queue(5);

    // Simulate wrapping around with concurrent operations
    std::atomic<int> counter(0);

    auto enqueueTask = [&queue, &counter]() {
        for (int i = 0; i < 5; ++i) {
            queue.enqueue(counter.fetch_add(1));
        }
    };

    auto dequeueTask = [&queue]() {
        for (int i = 0; i < 5; ++i) {
            queue.dequeue();
        }
    };

    // Threads enqueue and dequeue concurrently
    std::thread t1(enqueueTask);
    std::thread t2(enqueueTask);
    std::thread t3(dequeueTask);
    std::thread t4(dequeueTask);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    assert(queue.isEmpty() && "Queue should be empty after all dequeues");
}

void testThreadSafetyWithOverflow() {
    RingBufferQueue<int> queue(3);
    std::atomic<int> counter(0);

    // Enqueue in one thread
    auto enqueueTask = [&queue, &counter]() {
        for (int i = 0; i < 6; ++i) {
            try {
                queue.enqueue(counter.fetch_add(1));
            } catch (const std::overflow_error&) {
                // Ignore overflow error
            }
        }
    };

    std::thread t1(enqueueTask);
    std::thread t2(enqueueTask);

    t1.join();
    t2.join();

    assert(queue.size() == 3 && "Queue should not exceed its capacity of 3 items");
}

void testThreadSafetyWithMixedOps() {
    RingBufferQueue<int> queue(5);

    // Mixed enqueue and dequeue operations in multiple threads
    auto enqueueTask = [&queue]() {
        for (int i = 0; i < 5; ++i) {
            queue.enqueue(i);
        }
    };

    auto dequeueTask = [&queue]() {
        for (int i = 0; i < 5; ++i) {
            if (!queue.isEmpty()) {
                queue.dequeue();
            }
        }
    };

    std::thread t1(enqueueTask);
    std::thread t2(dequeueTask);
    std::thread t3(enqueueTask);
    std::thread t4(dequeueTask);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    assert(queue.isEmpty() && "Queue should be empty after all operations");
}

int main() {
	std::cout << "Test 1: Single-threaded enqueue and dequeue" << std::endl;
	testSingleThreadEnqueueDequeue();

	std::cout << "Test 2: Multi-threaded enqueue (two threads)" << std::endl;
    testMultiThreadedEnqueue();

    // Test 3: Multi-threaded dequeue (two threads)
    testMultiThreadedDequeue();

    // Test 4: Concurrency with wrap-around behavior
    testConcurrencyWithWrapAround();

    // Test 5: Thread safety with overflow handling
    testThreadSafetyWithOverflow();

    // Test 6: Thread safety with mixed enqueue and dequeue operations
    testThreadSafetyWithMixedOps();

    std::cout << "All tests passed!" << std::endl;
    return 0;
}

