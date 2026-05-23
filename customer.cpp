#include "Customer.h"

Customer::Customer() {
    id = 0;
    arrivalTime = 0;
    serviceTime = 0;
    waitingTime = 0;
    isVIP = false;
}

Customer::Customer(int id, int arr, int ser, bool vip) {
    this->id = id;
    this->arrivalTime = arr;
    this->serviceTime = ser;
    this->isVIP = vip;
    this->waitingTime = 0;
}

int Customer::getId() const { return id; }
int Customer::getArrivalTime() const { return arrivalTime; }
int Customer::getServiceTime() const { return serviceTime; }
int Customer::getWaitingTime() const { return waitingTime; }
bool Customer::getIsVIP() const { return isVIP; }

void Customer::calculateWaitingTime(int currentTime) {
    waitingTime = currentTime - arrivalTime;
}