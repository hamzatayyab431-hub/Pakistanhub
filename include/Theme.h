#ifndef THEME_H
#define THEME_H

#include <SFML/Graphics/Color.hpp>

namespace Theme {
    const sf::Color BG_DARK        = sf::Color(10,  14,  20,  255); // #0A0E14
    const sf::Color BG_MID         = sf::Color(15,  20,  30,  255); // #0F141E
    const sf::Color GLASS_FILL     = sf::Color(255, 255, 255, 18);  // white ~7% alpha
    const sf::Color GLASS_BORDER   = sf::Color(255, 255, 255, 45);  // white ~18% alpha
    const sf::Color GREEN_PRIMARY  = sf::Color(0,   200, 100, 255); // #00C864
    const sf::Color GREEN_GLOW     = sf::Color(0,   200, 100, 40);  // green at 16% for glow
    const sf::Color GREEN_DIM      = sf::Color(0,   160, 80,  255); // darker green for hover
    const sf::Color TEXT_PRIMARY   = sf::Color(240, 245, 250, 255); // near white
    const sf::Color TEXT_MUTED     = sf::Color(160, 170, 185, 255); // muted grey-blue
    const sf::Color TEXT_DIM       = sf::Color(90,  100, 115, 255); // very muted
    const sf::Color ACCENT_RED     = sf::Color(220, 70,  70,  255); // for errors
}

#endif // THEME_H
