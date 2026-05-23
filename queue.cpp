#include "Queue.h"

// ===== Node =====
Node::Node(Customer c) {
    data = c;
    next = nullptr;
}

// ===== Queue Constructor =====
Queue::Queue() {
    front = nullptr;
    rear = nullptr;
    count = 0;
}

// ===== Destructor =====
Queue::~Queue() {
    while (!isQueueEmpty()) {
        dequeueCustomer();
    }
}

// ===== isEmpty =====
bool Queue::isQueueEmpty() const {
    return count == 0;
}

// ===== size =====
int Queue::getQueueSize() const {
    return count;
}

// ===== enqueue =====
void Queue::enqueueCustomer(Customer c) {
    Node* newNode = new Node(c);

    if (isQueueEmpty()) {
        front = rear = newNode;
    }
    else if (c.getIsVIP()) {
        if (!front->data.getIsVIP()) {
            newNode->next = front;
            front = newNode;
        }
        else {
            Node* current = front;

            while (current->next != nullptr &&
                current->next->data.getIsVIP()) {
                current = current->next;
            }

            newNode->next = current->next;
            current->next = newNode;

            if (newNode->next == nullptr) {
                rear = newNode;
            }
        }
    }
    else {
        rear->next = newNode;
        rear = newNode;
    }

    count++;
}

// ===== dequeue =====
Customer Queue::dequeueCustomer() {
    if (isQueueEmpty()) {
        throw std::runtime_error("Error: Queue is empty!");
    }

    Node* temp = front;
    Customer c = temp->data;

    front = front->next;

    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
    count--;

    return c;
}

// ===== get at index =====
Customer Queue::getCustomerAt(int index) const {
    if (index < 0 || index >= count) {
        throw std::out_of_range("Error: Index out of bounds!");
    }

    Node* current = front;

    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    return current->data;
}

// ===== update waiting times =====
void Queue::updateWaitingTimes(int currentTime) {
    Node* current = front;

    while (current != nullptr) {
        current->data.calculateWaitingTime(currentTime);
        current = current->next;
    }
}