#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

// Semaphore class to control access to the shared resources
class Semaphore {
public:
    Semaphore(int count) : count(count) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        while (count <= 0) {
            condition.wait(lock);
        }
        --count;
    }

    void signal() {
        std::unique_lock<std::mutex> lock(mutex);
        ++count;
        condition.notify_one();
    }

private:
    std::mutex mutex;
    std::condition_variable condition;
    int count;
};
