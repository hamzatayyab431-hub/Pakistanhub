#ifndef GLASSPANEL_H
#define GLASSPANEL_H

#include <SFML/Graphics.hpp>

class GlassPanel {
public:
    static void draw(sf::RenderWindow& window, const sf::FloatRect& bounds, bool glowing = false, float outlineThickness = 1.0f, sf::Color outlineColor = sf::Color(255, 255, 255, 45));
};

#endif // GLASSPANEL_H
