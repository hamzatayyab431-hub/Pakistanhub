#pragma once
#include <SFML/Graphics.hpp>

namespace Theme {
    // Backgrounds
    inline const sf::Color BG_BASE       = {8,   12,  18,  255}; // #080C12
    inline const sf::Color BG_DEEP       = {5,   8,   13,  255}; // #05080D

    // Keep legacy aliases so untouched files that reference them still compile
    inline const sf::Color BG_DARK       = BG_BASE;
    inline const sf::Color BG_MID        = {12,  17,  25,  255};

    // Glass layers
    inline const sf::Color GLASS_1       = {255, 255, 255, 12};   // subtle fill
    inline const sf::Color GLASS_2       = {255, 255, 255, 20};   // hover fill
    inline const sf::Color GLASS_3       = {255, 255, 255, 32};   // active/elevated fill
    inline const sf::Color GLASS_BORDER  = {255, 255, 255, 35};   // panel border
    inline const sf::Color GLASS_TOP     = {255, 255, 255, 55};   // top highlight edge

    // Keep legacy alias
    inline const sf::Color GLASS_FILL    = GLASS_1;

    // Green accent family
    inline const sf::Color GREEN         = {0,   210, 100, 255};  // #00D264 — primary
    inline const sf::Color GREEN_DARK    = {0,   160, 75,  255};  // hover/pressed
    inline const sf::Color GREEN_DIM     = {0,   210, 100, 140};  // semi-transparent
    inline const sf::Color GREEN_GLOW    = {0,   210, 100, 28};   // ambient glow
    inline const sf::Color GREEN_GLOW2   = {0,   210, 100, 12};   // far glow

    // Legacy aliases
    inline const sf::Color GREEN_PRIMARY = GREEN;

    // Text
    inline const sf::Color TEXT_WHITE    = {235, 242, 250, 255};
    inline const sf::Color TEXT_MUTED    = {140, 155, 170, 255};
    inline const sf::Color TEXT_DIM      = {75,  88,  105, 255};
    inline const sf::Color TEXT_GREEN    = {0,   210, 100, 255};

    // Legacy aliases
    inline const sf::Color TEXT_PRIMARY  = TEXT_WHITE;

    // Semantic
    inline const sf::Color ERROR         = {220, 65,  65,  255};
    inline const sf::Color WARNING       = {240, 170, 40,  255};
    inline const sf::Color SUCCESS       = GREEN;

    // Legacy alias
    inline const sf::Color ACCENT_RED    = ERROR;

    // Overlays
    inline const sf::Color OVERLAY_DARK  = {0,   0,   0,   160};
    inline const sf::Color SCRIM         = {0,   0,   0,   200};
}
