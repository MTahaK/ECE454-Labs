// ECE454 A4 Part 2 Question 1
// By: Taha Khokhar, ID: 1005813069
// Group #30
// Lab Session 02
// Date: 2023-11-23

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <numeric>
#include <stdlib.h>
#include <time.h>

#define NUM_VALUES_TO_INSERT 10000
#define NUM_THREADS 8

struct Node
{
    int data;
    Node* next;
    std::mutex nodeMutex; // Fine-grained locking

    Node(int data) : data(data), next(nullptr) {}
};

class LinkedList
{
    public:
        Node* head;
        Node* tail;
        std::mutex listMutex; // Coarse-grained locking
    
        LinkedList() : head(nullptr), tail(nullptr) {}

        // Insert new node according to ascending order
        void regularInsert(int value){
            Node* newNode = new Node(value);
            if(head == nullptr){
                head = newNode;
                tail = newNode;
            }
            else if(head->data > value){
                newNode->next = head;
                head = newNode;
            }
            else{
                // Traverse through linked list until we find the correct position
                // adhering to the ascending order of the list.
                Node* curr = head;
                while(curr->next != nullptr && curr->next->data < value){
                    curr = curr->next;
                }
                newNode->next = curr->next;
                curr->next = newNode;
                if(newNode->next == nullptr){
                    tail = newNode;
                }
            }
        }

        // Coarse-grained locking insertion; acquires lock for entire list before
        // performing basic ascending-order insertion.
        void coarseGrainedInsert(int value) {
            std::lock_guard<std::mutex> lock(this->listMutex);
            regularInsert(value);
        }

        // Fine-grained locking insertion; acquires lock for each node before
        // performing basic ascending-order insertion.
        void fineGrainedInsert(int value) {
            Node* newNode = new Node(value);
            if(head == nullptr || value < head->data){
                std::lock_guard<std::mutex> lock(newNode->nodeMutex); // Acquire per-node lock
                newNode->next = head;
                head = newNode;
            } else {
                Node* prev = head;
                Node* curr = head->next;
                prev->nodeMutex.lock(); // Acquire per-node lock; cannot use lock guard, as greater control over locks are needed
                while(curr && value > curr->data){
                    prev->nodeMutex.unlock(); // Release lock on previous node, as no longer needed
                    prev = curr;
                    curr = curr->next;
                    prev->nodeMutex.lock(); // Acquire lock on next node in advancement
                }

                // Location for insertion found; lock for location is acquired
                newNode->next = curr;
                prev->next = newNode;
                prev->nodeMutex.unlock(); // Release lock on previous node
                if(curr){
                    curr->nodeMutex.unlock(); // Release lock on next node
                }
                // If insertion is at the end of the list, update tail
                if(newNode->next == nullptr){
                    tail = newNode;
                }
                
            }
        }
};

void threadInsert(LinkedList& list, std::vector<int> values, bool useFineGrained) {
    if (useFineGrained) {
        for (int value : values) {
            list.fineGrainedInsert(value);
        }
    } else {
        for (int value : values) {
            list.coarseGrainedInsert(value);
        }
    }
}

// Function to initialize a linked list with initial values. Used to restore original linked list
// state after each iteration of insertions.
void initializeList(LinkedList& list) {
    // Clear the list if not empty
    while (list.head != nullptr) {
        Node* temp = list.head;
        list.head = list.head->next;
        delete temp;
    }

    // Regular insertions
    int initialValues[] = {40, 50, 100, 120, 160, 180};
    for (int value : initialValues) {
        list.regularInsert(value);
    }
}


int main()
{
    LinkedList list1, list2;
    std::vector<long long> coarseTimes, fineTimes;
    std::vector<int> valuesToInsert;

    srand(time(NULL));
    // Initialize list with 1000 random values (seeded by time) to insert
    for(int i = 0; i < NUM_VALUES_TO_INSERT; ++i){
        valuesToInsert.push_back(rand() % NUM_VALUES_TO_INSERT);
    }
    
    std::cout<< "Number of threads: " << NUM_THREADS << std::endl;
    std::cout<< "Number of values to insert: " << NUM_VALUES_TO_INSERT << std::endl << std::endl;


    for (int i = 0; i < 1; i++) {
        initializeList(list1); // Reset list1 to initial state

        auto startCoarse = std::chrono::high_resolution_clock::now();
        // Generate vector of NUM_THREADS threads, each running threadInsert.

        std::vector<std::thread> coarseThreads;
        for (int i = 0; i < NUM_THREADS; i++) {
            coarseThreads.push_back(std::thread(threadInsert, std::ref(list1), valuesToInsert, false));
        }
        
        // join all threads
        for (auto& thread : coarseThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        auto endCoarse = std::chrono::high_resolution_clock::now();
        coarseTimes.push_back(std::chrono::duration_cast<std::chrono::microseconds>(endCoarse - startCoarse).count());
    }

    for (int i = 0; i < 1; i++) {
        initializeList(list2); // Reset list2 to initial state

        auto startFine = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> fineThreads;
        for (int i = 0; i < NUM_THREADS; i++){
            fineThreads.push_back(std::thread(threadInsert, std::ref(list2), valuesToInsert, true));
        }

        // join all threads
        for (auto& thread : fineThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

        auto endFine = std::chrono::high_resolution_clock::now();
        fineTimes.push_back(std::chrono::duration_cast<std::chrono::microseconds>(endFine - startFine).count());
    }

    // Get averages for each insertion type
    long long coarseAverage = std::accumulate(coarseTimes.begin(), coarseTimes.end(), 0LL) / coarseTimes.size(); 
    long long fineAverage = std::accumulate(fineTimes.begin(), fineTimes.end(), 0LL) / fineTimes.size();

    // !coarse-grained locking should be faster than fine-grained locking, as the latter requires locking and unlocking
    // !for each node in the list, while the former only requires locking and unlocking for the entire list. The higher
    // !the number of nodes in the list, the more time fine-grained locking will take.
    // !HOWEVER, the degree of contention between the threads in attempting to access the list will also affect the results.
    // !coarse-grained locking here completely locks the list, so the second thread will have to wait for the first thread,
    // !destroying any potential parallelism. Fine-grained locking allows for more parallelism, as the threads will only
    // !have to wait for each other when they are attempting to access the same node. Therefore, fine-grained locking may 
    // !be faster here despite the lower overhead of coarse-grained locking.

    
    // coarse-grained results w/ average:
    std::cout << "Coarse-grained locking times: ";
    for (int i = 0; i < coarseTimes.size(); ++i) {
        std::cout << coarseTimes[i] << " ";
    }
    // std::cout << std::endl << "Average time for coarse-grained insertion: " << coarseAverage << " microseconds" << std::endl << std::endl;
    // Give time in seconds
    std::cout << std::endl << "Average time for coarse-grained insertion: " << coarseAverage / 1000000.0 << " seconds" << std::endl << std::endl;
    
    // Fine-grained results w/ average:
    std::cout << "Fine-grained locking times: ";
    for (int i = 0; i < fineTimes.size(); ++i) {
        std::cout << fineTimes[i] << " ";
    }
    // std::cout << std::endl << "Average time for fine-grained insertion: " << fineAverage << " microseconds" << std::endl;
    std::cout << std::endl << "Average time for fine-grained insertion: " << fineAverage / 1000000.0 << " seconds" << std::endl;

    // // Check if lists are same
    // Node* curr1 = list1.head;
    // Node* curr2 = list2.head;
    // bool same = true;
    // while (curr1 && curr2) {
    //     if (curr1->data != curr2->data) {
    //         same = false;
    //         break;
    //     }
    //     curr1 = curr1->next;
    //     curr2 = curr2->next;
    // }
    // if (same) {
    //     std::cout << "Lists are the same" << std::endl;
    // } else {
    //     std::cout << "Lists are different" << std::endl;
    // }
    
    return 0;
}