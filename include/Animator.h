#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>

// ─── Lerp ────────────────────────────────────────────────────────────────────
struct Lerp {
    float current = 0.f;
    float target  = 0.f;
    float speed   = 8.f;   // exponential lerp speed factor (units/second)

    void setTarget(float t) { target = t; }
    void snap(float v)      { current = target = v; }

    void update(float dt) {
        current += (target - current) * std::min(1.f, speed * dt);
    }

    bool settled() const { return std::abs(current - target) < 0.5f; }
};

// ─── LerpColor ───────────────────────────────────────────────────────────────
struct LerpColor {
    sf::Color current = sf::Color::Transparent;
    sf::Color target  = sf::Color::Transparent;
    float speed = 8.f;

    void setTarget(sf::Color t) { target = t; }
    void snap(sf::Color v)      { current = target = v; }

    void update(float dt) {
        float f = std::min(1.f, speed * dt);
        current.r = static_cast<sf::Uint8>(current.r + (target.r - current.r) * f);
        current.g = static_cast<sf::Uint8>(current.g + (target.g - current.g) * f);
        current.b = static_cast<sf::Uint8>(current.b + (target.b - current.b) * f);
        current.a = static_cast<sf::Uint8>(current.a + (target.a - current.a) * f);
    }

    sf::Color get() const { return current; }
};

// ─── BlinkClock ──────────────────────────────────────────────────────────────
struct BlinkClock {
    sf::Clock clock;
    float interval = 0.53f;   // seconds between blinks
    bool  state    = true;

    bool tick() {
        if (clock.getElapsedTime().asSeconds() >= interval) {
            state = !state;
            clock.restart();
        }
        return state;
    }
};
