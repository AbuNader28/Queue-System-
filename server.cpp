#include "Server.h"

Server::Server() {
    busy = false;
    currentCustomer = nullptr;
    remainingTime = 0;
    totalServedCount = 0;
}

bool Server::isFree() const { return !busy; }
int Server::getRemainingTime() const { return remainingTime; }
int Server::getTotalServedCount() const { return totalServedCount; }

void Server::assignCustomer(Customer* c) {
    currentCustomer = c;
    remainingTime = c->getServiceTime();
    busy = true;
}

void Server::updateTime() {
    if (busy && remainingTime > 0) {
        remainingTime--;
    }
}

void Server::finishService() {
    busy = false;
    remainingTime = 0;
    totalServedCount++;
    currentCustomer = nullptr;
}