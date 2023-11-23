#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <numeric>

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
                    prev->nodeMutex.lock(); // Acquire lock on next node
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

void threadInsert(LinkedList& list, int value, bool useFineGrained) {
    if (useFineGrained) {
        list.fineGrainedInsert(value);
    } else {
        list.coarseGrainedInsert(value);
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


    for (int i = 0; i < 5; ++i) {
        initializeList(list1); // Reset list1 to initial state

        auto startCoarse = std::chrono::high_resolution_clock::now();
        std::thread coarseThread1(threadInsert, std::ref(list1), 65, false);
        std::thread coarseThread2(threadInsert, std::ref(list1), 77, false);

        coarseThread1.join();
        coarseThread2.join();

        auto endCoarse = std::chrono::high_resolution_clock::now();
        coarseTimes.push_back(std::chrono::duration_cast<std::chrono::microseconds>(endCoarse - startCoarse).count());
    }

    for (int i = 0; i < 5; ++i) {
        initializeList(list2); // Reset list2 to initial state

        auto startFine = std::chrono::high_resolution_clock::now();
        std::thread fineThread1(threadInsert, std::ref(list2), 65, true);
        std::thread fineThread2(threadInsert, std::ref(list2), 77, true);

        fineThread1.join();
        fineThread2.join();
        
        auto endFine = std::chrono::high_resolution_clock::now();
        fineTimes.push_back(std::chrono::duration_cast<std::chrono::microseconds>(endFine - startFine).count());
    }

    // Get averages for each insertion type
    long long coarseAverage = std::accumulate(coarseTimes.begin(), coarseTimes.end(), 0LL) / coarseTimes.size(); 
    long long fineAverage = std::accumulate(fineTimes.begin(), fineTimes.end(), 0LL) / fineTimes.size();

    std::cout << "Average time for coarse-grained insertion: " << coarseAverage << " microseconds" << std::endl;
    std::cout << "Average time for fine-grained insertion: " << fineAverage << " microseconds" << std::endl;

    return 0;
}