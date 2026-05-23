#ifndef QUEUE_H
#define QUEUE_H

#include "Customer.h"
#include <stdexcept>

class Node {
public:
    Customer data;
    Node* next;

    Node(Customer c);
};

class Queue {
private:
    Node* front;
    Node* rear;
    int count;

public:
    Queue();
    ~Queue();

    bool isQueueEmpty() const;
    int getQueueSize() const;

    void enqueueCustomer(Customer c);
    Customer dequeueCustomer();

    Customer getCustomerAt(int index) const;

    void updateWaitingTimes(int currentTime);
};

#endif