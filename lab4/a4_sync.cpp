#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <time.h>

#define NUM_CUSTOMERS 10
#define SERVICE_TIME_MAX 30
// Semaphore class to control access to the shared resources
// USE SEMAPHORE TO LOCK QUEUE, NOT TELLERS.
// CONSIDER THAT THE TELLERS ARE THE CONSUMERS AND THE CUSTOMERS ARE PRODUCERS.
// THEREFORE, LOCK ACCESS TO THE QUEUE, NOT THE TELLERS.

// ^ NO LONGER GOING WITH THIS IMPLEMENTATION: CUSTOMERS ARE THREADS, 3 TELLERS ARE LOCKED BEHIND SEMAPHORES.
std::mutex coutMutex;
class Semaphore{
public:
    Semaphore(int count) : count(count) {}

    void wait(){
        std::unique_lock<std::mutex> lock(mutex);
        while (count <= 0){
            condition.wait(lock);
        }
        --count;
    }

    void signal(){
        std::unique_lock<std::mutex> lock(mutex);
        ++count;
        condition.notify_one();
    }

private:
    std::mutex mutex;
    std::condition_variable condition;
    int count;
};

class Customer{
public:
    Customer(int id) : id(id), serviceTime(rand() % SERVICE_TIME_MAX) {}

    int id;
    int serviceTime; // Random service time less than SERVICE_TIME_MAX

    // Set the thread servicing this customer
    void setServiceThread(std::thread::id threadId){
        servicingThreadId = threadId;
    }

    // Get the thread id servicing this customer
    std::thread::id getServiceThread() const{
        return servicingThreadId;
    }

private:
    std::thread::id servicingThreadId; // Stores the ID of the servicing thread
};

class Teller{
public:
    Teller(int id) : id(id) {}

    int getId() const{
        return id;
    }

private:
    int id;
};

void serviceCustomer(Customer &customer, Semaphore &tellerSemaphore, Teller &teller){
    tellerSemaphore.wait();

    // Must guard cout with a mutex to prevent interleaved output
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Customer " << customer.id << " is being serviced by teller " << teller.getId() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(customer.serviceTime));

    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Customer " << customer.id << " has finished being serviced by teller " << teller.getId() << std::endl;
    }

    tellerSemaphore.signal(); // Release the teller
}

int main(void){
    Semaphore tellerLock(3); // Semaphore to control access to the queue
    std::time_t start, end;

    std::vector<Teller> tellers;
    for (int i = 0; i < 3; ++i){
        tellers.emplace_back(i);
    }
    // Create vector of customers to pass to threads as non-local objects
    std::vector<Customer> customers;
    for (int i = 0; i < NUM_CUSTOMERS; ++i){
        customers.emplace_back(i);
        std::cout<< "Customer " << customers[i].id << " Service time: " << customers[i].serviceTime<<std::endl;
    }
    std::vector<std::thread> customerThreads;

    start = time(NULL);
    for (int i = 0; i < NUM_CUSTOMERS; i++){
        customerThreads.push_back(std::thread(serviceCustomer, std::ref(customers[i]), std::ref(tellerLock), std::ref(tellers[i % 3])));
    }

    for (auto &thread : customerThreads){
        if (thread.joinable()){
            thread.join();
        }
    }
    end = time(NULL);

    std::time_t elapsed = end - start;
    std::time_t average = elapsed / NUM_CUSTOMERS;
    std::cout << "Total time spent: " <<  elapsed << " Average time spent waiting and being serviced: " << average << " seconds" << std::endl;
    return 0;
}