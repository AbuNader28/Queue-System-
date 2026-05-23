#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <string>
#include "customer.h"
#include "queue.h"
#include "server.h"

// ─── Layout constants ────────────────────────────────────────────────────────
const unsigned int WIN_W = 900;
const unsigned int WIN_H = 600;
const float        PANEL = 220.f;   // left info-panel width

// ─── Colour palette ──────────────────────────────────────────────────────────
namespace Palette {
    const sf::Color BG(15, 17, 26);
    const sf::Color PANEL_BG(22, 26, 42);
    const sf::Color ACCENT(99, 179, 237);  // sky-blue
    const sf::Color VIP(255, 200, 80);  // gold
    const sf::Color NORMAL(72, 149, 239);  // blue
    const sf::Color SERVER_ON(80, 200, 120);  // green
    const sf::Color SERVER_OFF(100, 100, 120);  // grey
    const sf::Color TEXT_HI(230, 235, 255);
    const sf::Color TEXT_LO(130, 140, 170);
    const sf::Color BAR_BG(35, 40, 60);
}

// ─── GUI class ───────────────────────────────────────────────────────────────
class GUI {
public:
    // Construct with references to simulation objects + optional font path
    GUI(Queue& q, Server& s, const std::string& fontPath = "");

    // Returns false once the window is closed
    bool isOpen() const;

    // Poll & handle window events — call once per simulation tick
    void handleEvents();

    // Redraw the entire frame
    //   currentTime : simulation clock value
    //   lastEvent   : short human-readable description of what just happened
    void render(int currentTime, const std::string& lastEvent = "");

private:
    // ── SFML objects ──────────────────────────────────────────────────────
    sf::RenderWindow window_;
    sf::Font         font_;
    bool             fontLoaded_;

    // ── Simulation state (non-owning references) ──────────────────────────
    Queue& queue_;
    Server& server_;

    // ── Drawing helpers ───────────────────────────────────────────────────
    void drawBackground();
    void drawInfoPanel(int currentTime, const std::string& lastEvent);
    void drawQueueArea();
    void drawServerArea();

    void drawCustomerCard(float x, float y, float w, float h,
        const Customer& c, bool highlight = false);

    void drawProgressBar(float x, float y, float w, float h,
        float ratio, sf::Color fill);

    // SFML 2.6: sf::Text constructor takes no arguments —
    // font and string are set via setFont() / setString()
    sf::Text makeText(const std::string& str, unsigned int charSize,
        sf::Color colour, float x, float y);
};

#endif // GUI_H