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
};

int main()
{
    LinkedList myList;

    // Regular insertions
    int initialValues[] = {40, 50, 100, 120, 160, 180};
    for (int value : initialValues) {
        myList.regularInsert(value);
    }
    // Traverse through linked list and print values in readable format
    Node* curr = myList.head;
    std::cout << "Regular insertions: " << std::endl;
    while(curr != nullptr){
        std::cout << curr->data << " ";
        curr = curr->next;
    }
    std::cout<< std::endl;
}