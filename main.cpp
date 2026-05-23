#include <iostream>
#include <cstdlib>  // rand
#include <ctime>    // time
#include "Customer.h"
#include "Server.h"
#include "Queue.h"
#include "gui.h"

using namespace std;

int main() {

    Queue queue;
    Server server;
    GUI gui(queue, server);

    srand(static_cast<unsigned int>(time(nullptr)));

    int simTime = 0;
    int customerId = 1;
    string lastEvent = "Simulation started";

    sf::Clock clock;
    float accumulator = 0.f;
    const float tickRate = 0.8f; // 800 ms per simulation step

    while (gui.isOpen())
    {
        // Handle window events
        gui.handleEvents();

        // Time handling (no freezing)
        float dt = clock.restart().asSeconds();
        accumulator += dt;

        // ── Simulation tick ─────────────────────────────
        if (accumulator >= tickRate)
        {
            accumulator = 0.f;

            if (simTime < 800)
            {
                int chance = rand() % 100;

                if (chance < 40)
                {
                    int serviceTime = (rand() % 5) + 1;
                    bool isVIP = (rand() % 10 < 2);

                    Customer newCustomer(customerId++, simTime, serviceTime, isVIP);
                    queue.enqueueCustomer(newCustomer);

                    lastEvent = string(isVIP ? "[VIP] " : "") +
                        "Customer #" + to_string(customerId - 1) +
                        " arrived (svc=" + to_string(serviceTime) + ")";

                    cout << lastEvent << "\n";
                }

                queue.updateWaitingTimes(simTime);

                if (server.isFree() && !queue.isQueueEmpty())
                {
                    Customer c = queue.dequeueCustomer();
                    server.assignCustomer(new Customer(c));

                    lastEvent = "Server picked up Customer #" + to_string(c.getId());
                }

                server.updateTime();

                if (!server.isFree() && server.getRemainingTime() == 0)
                {
                    server.finishService();
                    lastEvent = "Service complete — server now free";
                }

                cout << "Time: " << simTime
                    << " | Queue: " << queue.getQueueSize()
                    << " | Server free: " << server.isFree()
                    << "\n";

                simTime++;
            }
            else
            {
                lastEvent = "Simulation finished (tick 15)";
            }
        }

        // Render every frame (important!)
        gui.render(simTime, lastEvent);
    }

    return 0;
}