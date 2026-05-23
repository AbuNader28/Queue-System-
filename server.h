#ifndef SERVER_H
#define SERVER_H

#include "Customer.h"

class Server {
private:
    bool busy;
    Customer* currentCustomer;
    int remainingTime;
    int totalServedCount;

public:
    Server();

    bool isFree() const;
    int getRemainingTime() const;
    int getTotalServedCount() const;

    void assignCustomer(Customer* c);
    void updateTime();
    void finishService();
};

#endif