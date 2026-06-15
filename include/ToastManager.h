#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Animator.h"
#include "Theme.h"

enum class ToastType { SUCCESS, ERROR, WARNING, INFO };

struct Toast {
    std::string  title;
    std::string  subtitle;
    ToastType    type      = ToastType::INFO;
    float        lifetime  = 2.8f;   // seconds until slide-out begins
    float        elapsed   = 0.f;
    bool         slidingOut = false;
    Lerp         xLerp;              // x position animation (1290 → 950 → 1290)
    Lerp         alphaLerp;          // alpha fade
};

class ToastManager {
public:
    static ToastManager& instance() {
        static ToastManager inst;
        return inst;
    }

    void push(const std::string& title,
              const std::string& subtitle = "",
              ToastType type = ToastType::INFO);

    void update(float dt);
    void draw(sf::RenderWindow& window, sf::Font& font);

private:
    static constexpr float CARD_W  = 320.f;
    static constexpr float CARD_H  = 56.f;
    static constexpr float CARD_X_IN   = 950.f;   // on-screen resting x
    static constexpr float CARD_X_OUT  = 1290.f;  // off-screen x
    static constexpr float BOTTOM_Y    = 700.f;
    static constexpr float STACK_GAP   = 8.f;

    std::vector<Toast> toasts;

    ToastManager() = default;
    sf::Color barColor(ToastType t) const;
};
