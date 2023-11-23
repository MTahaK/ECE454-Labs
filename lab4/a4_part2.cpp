#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

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


int main()
{
    LinkedList list1;
    LinkedList list2;

    // Regular insertions
    int initialValues[] = {40, 50, 100, 120, 160, 180};
    for (int value : initialValues) {
        list1.regularInsert(value);
        list2.regularInsert(value);
    }
    // Traverse through linked list and print values in readable format
    Node* curr = list1.head;
    std::cout << "Regular insertions (data for both lists are identical currently): " << std::endl;
    while(curr != nullptr){
        std::cout << curr->data << " ";
        curr = curr->next;
    }
    std::cout<< std::endl;

    // Course-grained insertions

    std::thread thread1(threadInsert, std::ref(list1), 65, false);
    std::thread thread2(threadInsert, std::ref(list1), 77, false);

    thread1.join();
    thread2.join();

    std::cout << "Course-Grained insertions: " << std::endl;
    curr = list1.head;
    while(curr != nullptr){
        std::cout << curr->data << " ";
        curr = curr->next;
    }
    std::cout<< std::endl;

    std::thread thread3(threadInsert, std::ref(list2), 65, true);
    std::thread thread4(threadInsert, std::ref(list2), 77, true);

    thread3.join();
    thread4.join();

    std::cout << "Fine-Grained insertions: " << std::endl;
    curr = list2.head;
    while(curr != nullptr){
        std::cout << curr->data << " ";
        curr = curr->next;
    }
    std::cout<< std::endl;
}