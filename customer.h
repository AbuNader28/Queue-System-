#ifndef CUSTOMER_H
#define CUSTOMER_H

class Customer {
private:
    int id;
    int arrivalTime;
    int serviceTime;
    int waitingTime;
    bool isVIP;

public:
    Customer();
    Customer(int id, int arr, int ser, bool vip);

    int getId() const;
    int getArrivalTime() const;
    int getServiceTime() const;
    int getWaitingTime() const;
    bool getIsVIP() const;

    void calculateWaitingTime(int currentTime);
};

#endif