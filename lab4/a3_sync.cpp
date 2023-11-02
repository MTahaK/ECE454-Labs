#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>

// Semaphore class to control access to the shared resources
// USE SEMAPHORE TO LOCK QUEUE, NOT TELLERS.
// CONSIDER THAT THE TELLERS ARE THE CONSUMERS AND THE CUSTOMERS ARE PRODUCERS.
// THEREFORE, LOCK ACCESS TO THE QUEUE, NOT THE TELLERS.
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
    int id;
    void setServiceThread(std::thread::id threadId) {
        servicingThreadId = threadId;
    }

    // Get the thread id servicing this customer
    std::thread::id getServiceThread() const {
        return servicingThreadId;
    }

private:
    std::thread::id servicingThreadId;
};

int serviceCustomer(Customer &customer, Teller &teller) {
    // Service the customer
    if(teller.isAvailable) teller.isAvailable = false;
    else return 1; // Teller is busy
    std::cout << "Teller " << teller.id << " started serving customer " << customer.id << " on thread # " << std::this_thread::get_id() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(customer.serviceTime));
    std::cout << "Teller " << teller.id << " finished serving customer " << customer.id << " on thread # " << std::this_thread::get_id() << std::endl;

    // Set the teller to available
    teller.isAvailable = true;

    return 0;
}

int main(void){
    Semaphore queue_lock(1); // Semaphore to control access to the queue

    std::vector<Teller> tellerVector;
    for(int i = 0; i < 3; i++){
        Teller teller;
        teller.id = i;
        teller.isAvailable = true;
        tellerVector.push_back(teller);
    }
    
    // Create 10 customers in a queue to facilitate FIFO behaviour of servicing
    std::queue<Customer> customerQueue;
    for(int i = 0; i < 10; i++){
        Customer customer(i);
        customerQueue.push(customer);
    }
    // // Iterate through customer queue, print out service times, but re-add customer to queue
    // while(!customerQueue.empty()){
    //     std::cout << "Customer " << customerQueue.front().id << " service time: " << customerQueue.front().serviceTime << std::endl;
    //     customerQueue.pop();
    // }

    
    
    return 0;
}