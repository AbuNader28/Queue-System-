#include "gui.h"
#include <algorithm>  // std::min, std::clamp
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────
GUI::GUI(Queue& q, Server& s, const std::string& fontPath)
    : queue_(q), server_(s), fontLoaded_(false)
{
    // SFML 2.6 — VideoMode takes (width, height) directly, no braces
    window_.create(
        sf::VideoMode(WIN_W, WIN_H),
        "Queue Simulation",
        sf::Style::Titlebar | sf::Style::Close
    );
    window_.setFramerateLimit(60);

    // Try caller-supplied path first, then common fallbacks
    std::vector<std::string> candidates = {
        fontPath,
        "arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/calibri.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
    };

    for (const auto& path : candidates) {
        if (!path.empty() && font_.loadFromFile(path)) {   // SFML 2.6: loadFromFile
            fontLoaded_ = true;
            break;
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Public interface
// ─────────────────────────────────────────────────────────────────────────────
bool GUI::isOpen() const {
    return window_.isOpen();
}

void GUI::handleEvents() {
    sf::Event event;                         // SFML 2.6: plain sf::Event, not optional
    while (window_.pollEvent(event)) {       // SFML 2.6: pollEvent(event) with ref
        if (event.type == sf::Event::Closed) {
            window_.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape)
                window_.close();
        }
    }
}

void GUI::render(int currentTime, const std::string& lastEvent) {
    window_.clear(Palette::BG);

    drawBackground();
    drawInfoPanel(currentTime, lastEvent);
    drawQueueArea();
    drawServerArea();

    window_.display();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Private helpers
// ─────────────────────────────────────────────────────────────────────────────

void GUI::drawBackground() {
    // Vertical separator between left panel and main area
    sf::RectangleShape sep(sf::Vector2f(1.f, static_cast<float>(WIN_H)));
    sep.setPosition(PANEL, 0.f);             // SFML 2.6: setPosition(x, y)
    sep.setFillColor(sf::Color(50, 55, 80));
    window_.draw(sep);

    // Faint horizontal grid lines in the main area
    for (int y = 0; y < static_cast<int>(WIN_H); y += 60) {
        sf::RectangleShape line(sf::Vector2f(static_cast<float>(WIN_W) - PANEL, 1.f));
        line.setPosition(PANEL + 1.f, static_cast<float>(y));
        line.setFillColor(sf::Color(30, 35, 55));
        window_.draw(line);
    }
}

// ── Left panel: clock, stats, legend, last-event log ─────────────────────────
void GUI::drawInfoPanel(int currentTime, const std::string& lastEvent) {
    // Panel background
    sf::RectangleShape bg(sf::Vector2f(PANEL, static_cast<float>(WIN_H)));
    bg.setFillColor(Palette::PANEL_BG);
    window_.draw(bg);

    if (!fontLoaded_) return;

    // Title
    sf::Text title = makeText("QUEUE SIM", 18, Palette::ACCENT, 14.f, 16.f);
    title.setLetterSpacing(3.f);
    window_.draw(title);

    sf::RectangleShape underline(sf::Vector2f(PANEL - 28.f, 2.f));
    underline.setPosition(14.f, 44.f);
    underline.setFillColor(Palette::ACCENT);
    window_.draw(underline);

    // Clock
    window_.draw(makeText("TICK", 11, Palette::TEXT_LO, 14.f, 56.f));
    window_.draw(makeText(std::to_string(currentTime), 36, Palette::TEXT_HI, 14.f, 70.f));

    // Queue size
    window_.draw(makeText("IN QUEUE", 11, Palette::TEXT_LO, 14.f, 120.f));
    window_.draw(makeText(std::to_string(queue_.getQueueSize()), 28,
        Palette::NORMAL, 14.f, 134.f));

    // Server status
    window_.draw(makeText("SERVER", 11, Palette::TEXT_LO, 14.f, 178.f));
    bool free = server_.isFree();
    window_.draw(makeText(free ? "FREE" : "BUSY", 22,
        free ? Palette::SERVER_ON : sf::Color(220, 80, 80),
        14.f, 194.f));
    if (!free) {
        window_.draw(makeText("remaining: " +
            std::to_string(server_.getRemainingTime()) + " ticks",
            11, Palette::TEXT_LO, 14.f, 224.f));
    }

    // Total served
    window_.draw(makeText("TOTAL SERVED", 11, Palette::TEXT_LO, 14.f, 260.f));
    window_.draw(makeText(std::to_string(server_.getTotalServedCount()),
        28, Palette::SERVER_ON, 14.f, 274.f));

    // Legend
    float ly = 340.f;
    window_.draw(makeText("LEGEND", 11, Palette::TEXT_LO, 14.f, ly));
    ly += 18.f;

    sf::RectangleShape vipDot(sf::Vector2f(12.f, 12.f));
    vipDot.setPosition(14.f, ly + 2.f);
    vipDot.setFillColor(Palette::VIP);
    window_.draw(vipDot);
    window_.draw(makeText("VIP customer", 12, Palette::TEXT_HI, 32.f, ly));
    ly += 22.f;

    sf::RectangleShape normDot(sf::Vector2f(12.f, 12.f));
    normDot.setPosition(14.f, ly + 2.f);
    normDot.setFillColor(Palette::NORMAL);
    window_.draw(normDot);
    window_.draw(makeText("Regular customer", 12, Palette::TEXT_HI, 32.f, ly));

    // Last-event log box
    if (!lastEvent.empty()) {
        float logY = static_cast<float>(WIN_H) - 80.f;

        sf::RectangleShape logBg(sf::Vector2f(PANEL - 16.f, 64.f));
        logBg.setPosition(8.f, logY);
        logBg.setFillColor(sf::Color(30, 35, 55));
        logBg.setOutlineThickness(1.f);
        logBg.setOutlineColor(sf::Color(60, 70, 100));
        window_.draw(logBg);

        window_.draw(makeText("LAST EVENT", 10, Palette::TEXT_LO, 14.f, logY + 6.f));

        // Simple word-wrap at 28 characters
        std::string line1 = lastEvent.substr(0, 28);
        std::string line2 = lastEvent.size() > 28 ? lastEvent.substr(28) : "";
        window_.draw(makeText(line1, 11, Palette::ACCENT, 14.f, logY + 22.f));
        if (!line2.empty())
            window_.draw(makeText(line2, 11, Palette::ACCENT, 14.f, logY + 38.f));
    }
}

// ── Main area top: queue cards ────────────────────────────────────────────────
void GUI::drawQueueArea() {
    float areaX = PANEL + 10.f;
    float areaW = WIN_W - PANEL - 20.f;

    if (fontLoaded_) {
        window_.draw(makeText("WAITING QUEUE", 12, Palette::TEXT_LO, areaX, 10.f));

        sf::RectangleShape uh(sf::Vector2f(areaW, 1.f));
        uh.setPosition(areaX, 28.f);
        uh.setFillColor(sf::Color(50, 55, 80));
        window_.draw(uh);
    }

    if (queue_.isQueueEmpty()) {
        if (fontLoaded_)
            window_.draw(makeText("( empty )", 13, Palette::TEXT_LO,
                areaX + 10.f, 42.f));
        return;
    }

    const int   MAX_CARDS = 8;
    const float CARD_W = 148.f;
    const float CARD_H = 80.f;
    const float GAP = 10.f;

    int n = std::min(queue_.getQueueSize(), MAX_CARDS);
    for (int i = 0; i < n; ++i) {
        Customer c = queue_.getCustomerAt(i);
        int   col = i % 4;
        int   row = i / 4;
        float cx = areaX + col * (CARD_W + GAP);
        float cy = 36.f + row * (CARD_H + GAP);
        drawCustomerCard(cx, cy, CARD_W, CARD_H, c, i == 0);
    }

    // Overflow badge
    if (queue_.getQueueSize() > MAX_CARDS && fontLoaded_) {
        std::string more = "+" + std::to_string(queue_.getQueueSize() - MAX_CARDS) + " more";
        window_.draw(makeText(more, 12, Palette::TEXT_LO,
            areaX + areaW - 70.f, 36.f));
    }
}

// ── Main area bottom: server slot + stats bar ─────────────────────────────────
void GUI::drawServerArea() {
    float areaX = PANEL + 10.f;
    float areaW = WIN_W - PANEL - 20.f;
    float startY = 220.f;

    if (fontLoaded_) {
        window_.draw(makeText("SERVER", 12, Palette::TEXT_LO, areaX, startY));

        sf::RectangleShape uh(sf::Vector2f(areaW, 1.f));
        uh.setPosition(areaX, startY + 18.f);
        uh.setFillColor(sf::Color(50, 55, 80));
        window_.draw(uh);
    }

    float slotX = areaX;
    float slotY = startY + 24.f;
    float slotW = 280.f;
    float slotH = 100.f;

    sf::RectangleShape slot(sf::Vector2f(slotW, slotH));
    slot.setPosition(slotX, slotY);
    slot.setFillColor(Palette::BAR_BG);
    slot.setOutlineThickness(1.5f);
    slot.setOutlineColor(server_.isFree()
        ? sf::Color(50, 55, 80)
        : Palette::SERVER_ON);
    window_.draw(slot);

    if (server_.isFree()) {
        if (fontLoaded_)
            window_.draw(makeText("Idle - waiting for customer",
                12, Palette::TEXT_LO,
                slotX + 10.f, slotY + slotH / 2.f - 8.f));
    }
    else {
        if (fontLoaded_) {
            window_.draw(makeText("SERVING CUSTOMER", 11,
                Palette::SERVER_ON, slotX + 10.f, slotY + 8.f));
            window_.draw(makeText("Remaining ticks: " +
                std::to_string(server_.getRemainingTime()),
                13, Palette::TEXT_HI, slotX + 10.f, slotY + 30.f));
        }

        // Progress bar — service time is 1-5 ticks as per simulation setup
        const float MAX_SERVICE = 5.f;
        float ratio = 1.f - (server_.getRemainingTime() / MAX_SERVICE);
        // std::clamp not available in C++14; use manual clamp for compatibility
        if (ratio < 0.f) ratio = 0.f;
        if (ratio > 1.f) ratio = 1.f;
        drawProgressBar(slotX + 10.f, slotY + 54.f,
            slotW - 20.f, 14.f, ratio, Palette::SERVER_ON);
    }

    // Stats bar at the very bottom
    float statsY = static_cast<float>(WIN_H) - 55.f;

    sf::RectangleShape statsBar(sf::Vector2f(static_cast<float>(WIN_W), 55.f));
    statsBar.setPosition(0.f, statsY);
    statsBar.setFillColor(sf::Color(18, 22, 36));
    window_.draw(statsBar);

    sf::RectangleShape topLine(sf::Vector2f(static_cast<float>(WIN_W), 1.f));
    topLine.setPosition(0.f, statsY);
    topLine.setFillColor(sf::Color(45, 50, 75));
    window_.draw(topLine);

    if (fontLoaded_) {
        // Helper lambda for a label+value pair
        auto stat = [&](const std::string& label,
            const std::string& val, float x)
            {
                window_.draw(makeText(label, 10, Palette::TEXT_LO, x, statsY + 8.f));
                window_.draw(makeText(val, 16, Palette::TEXT_HI, x, statsY + 24.f));
            };

        stat("QUEUE SIZE", std::to_string(queue_.getQueueSize()), PANEL + 20.f);
        stat("SERVED", std::to_string(server_.getTotalServedCount()), PANEL + 160.f);
        stat("SERVER STATUS", server_.isFree() ? "FREE" : "BUSY", PANEL + 300.f);
    }
}

// ── Single customer card ──────────────────────────────────────────────────────
void GUI::drawCustomerCard(float x, float y, float w, float h,
    const Customer& c, bool highlight)
{
    sf::Color borderCol = c.getIsVIP() ? Palette::VIP : Palette::NORMAL;
    sf::Color bgCol = highlight
        ? sf::Color(35, 42, 68)
        : sf::Color(28, 33, 52);

    sf::RectangleShape card(sf::Vector2f(w, h));
    card.setPosition(x, y);
    card.setFillColor(bgCol);
    card.setOutlineThickness(1.5f);
    card.setOutlineColor(borderCol);
    window_.draw(card);

    // Coloured left stripe
    sf::RectangleShape stripe(sf::Vector2f(4.f, h));
    stripe.setPosition(x, y);
    stripe.setFillColor(borderCol);
    window_.draw(stripe);

    if (!fontLoaded_) return;

    std::string tag = c.getIsVIP() ? "[VIP] " : "";
    window_.draw(makeText(tag + "ID #" + std::to_string(c.getId()),
        12, borderCol, x + 10.f, y + 6.f));

    window_.draw(makeText("Arrived: t=" + std::to_string(c.getArrivalTime()),
        11, Palette::TEXT_LO, x + 10.f, y + 26.f));

    window_.draw(makeText("Service: " + std::to_string(c.getServiceTime()) + " ticks",
        11, Palette::TEXT_LO, x + 10.f, y + 42.f));

    window_.draw(makeText("Wait: " + std::to_string(c.getWaitingTime()) + " ticks",
        11, Palette::TEXT_HI, x + 10.f, y + 58.f));
}

// ── Progress bar ──────────────────────────────────────────────────────────────
void GUI::drawProgressBar(float x, float y, float w, float h,
    float ratio, sf::Color fill)
{
    sf::RectangleShape bg(sf::Vector2f(w, h));
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color(40, 45, 65));
    window_.draw(bg);

    if (ratio > 0.f) {
        sf::RectangleShape bar(sf::Vector2f(w * ratio, h));
        bar.setPosition(x, y);
        bar.setFillColor(fill);
        window_.draw(bar);
    }
}

// ── Text factory ──────────────────────────────────────────────────────────────
sf::Text GUI::makeText(const std::string& str, unsigned int charSize,
    sf::Color colour, float x, float y)
{
    sf::Text t;                  // SFML 2.6: default constructor
    t.setFont(font_);            // SFML 2.6: setFont() separately
    t.setString(str);
    t.setCharacterSize(charSize);
    t.setFillColor(colour);
    t.setPosition(x, y);        // SFML 2.6: setPosition(x, y)
    return t;
}