#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

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

class Customer {
public:
    Customer(int id) : id(id), serviceTime(rand() % 30) {}

    int id;
    int serviceTime; // Random service time less than 30 seconds

    // Set the thread servicing this customer
    void setServiceThread(std::thread::id threadId) {
        servicingThreadId = threadId;
    }

    // Get the thread id servicing this customer
    std::thread::id getServiceThread() const {
        return servicingThreadId;
    }

private:
    std::thread::id servicingThreadId; // Stores the ID of the servicing thread
};

class Teller {
public:
    Teller() : isAvailable(true) {}

    bool isAvailable;
    // Additional teller-related methods and attributes could be here.
};

void serviceCustomer(Customer &customer, Teller &teller) {
    // Service the customer
    std::cout << "Teller " << std::this_thread::get_id() << " started serving customer " << customer.id << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(customer.serviceTime));
    std::cout << "Teller " << std::this_thread::get_id() << " finished serving customer " << customer.id << std::endl;

    // Set the teller to available
    teller.isAvailable = true;

}