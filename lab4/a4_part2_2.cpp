#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>

// Spinlock class definition
class Spinlock {
private:
    // Atomic boolean to represent the lock state
    std::atomic<bool> locked;

public:
    // Constructor initializes the lock to be unlocked
    Spinlock() : locked(false) {}

    // Lock function for acquiring the spinlock
    void lock() {
        bool expected = false;
        // Continuously tries to set the lock from false to true
        // If another thread has it locked, this will fail and loop until it can lock
        while (!locked.compare_exchange_strong(expected, true, std::memory_order_acquire, std::memory_order_relaxed)) {
            expected = false; // Reset expected value after a failed exchange
        }
    }

    // Unlock function for releasing the spinlock
    void unlock() {
        // Unlocks by setting the atomic boolean to false
        locked.store(false, std::memory_order_release);
    }

    // Public method to check if the lock is currently held
    bool isLocked() const {
        return locked.load(std::memory_order_relaxed);
    }
};

// Function executed by each thread
void Task(Spinlock& spinlock, int& counter, std::chrono::microseconds& spinTime, std::chrono::microseconds& totalTime) {
    // Record the start time of the total operation
    auto totalStart = std::chrono::high_resolution_clock::now();
    // Also record the start time for the spinning phase
    auto spinStart = std::chrono::high_resolution_clock::now();

    // Loop to acquire the spinlock
    while (true) {
        // If the lock is not held, attempt to lock it
        if (!spinlock.isLocked()) {
            spinlock.lock();
            // Once the lock is acquired, record the end of the spinning phase
            auto spinEnd = std::chrono::high_resolution_clock::now();
            // Calculate the duration of the spinning phase
            spinTime = std::chrono::duration_cast<std::chrono::microseconds>(spinEnd - spinStart);
            break;
        }
        ++counter; // Increment the counter each time the lock is checked and found locked
        std::this_thread::yield(); // Yield to other threads
    }

    // Simulate a task by sleeping for 5 milliseconds
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    spinlock.unlock(); // Unlock after completing the task

    // Record the end time of the total operation
    auto totalEnd = std::chrono::high_resolution_clock::now();
    // Calculate the total duration of the operation
    totalTime = std::chrono::duration_cast<std::chrono::microseconds>(totalEnd - totalStart);
}

// Main function
int main() {
    // Define the number of runs and threads
    const int numRuns = 5;
    const int numThreads = 5;

    // Vectors to store spin times and total times for each thread across runs
    std::vector<std::vector<std::chrono::microseconds>> spinTimes(numThreads, std::vector<std::chrono::microseconds>(numRuns));
    std::vector<std::vector<std::chrono::microseconds>> totalTimes(numThreads, std::vector<std::chrono::microseconds>(numRuns));

    // Perform multiple runs
    for (int run = 0; run < numRuns; ++run) {
        Spinlock spinlock; // Create a spinlock
        std::vector<std::thread> threads; // Vector to hold threads
        std::vector<int> counters(numThreads, 0); // Counter for each thread

        // Create threads and start them
        for (int i = 0; i < numThreads; ++i) {
            threads.push_back(std::thread(Task, std::ref(spinlock), std::ref(counters[i]), std::ref(spinTimes[i][run]), std::ref(totalTimes[i][run])));
        }

        // Wait for all threads to complete
        for (auto& thread : threads) {
            thread.join();
        }
    }

    // Calculate average spin and total times
    std::vector<std::chrono::microseconds> avgSpinTimes(numThreads);
    std::vector<std::chrono::microseconds> avgTotalTimes(numThreads);
    for (int i = 0; i < numThreads; ++i) {
        for (int run = 0; run < numRuns; ++run) {
            avgSpinTimes[i] += spinTimes[i][run];
            avgTotalTimes[i] += totalTimes[i][run];
        }
        avgSpinTimes[i] /= numRuns; // Compute average spin time
        avgTotalTimes[i] /= numRuns; // Compute average total time
    }

    // Output the results in markdown table format
    std::cout << "| Thread # | Avg Spin Time (µs) | Avg Total Time (µs) |\n";
    std::cout << "|----------|---------------------|---------------------|\n";
    for (int i = 0; i < numThreads; ++i) {
        std::cout << "| " << i + 1 << "        | " << avgSpinTimes[i].count() << "               | " << avgTotalTimes[i].count() << "               |\n";
    }

    return 0;
}
