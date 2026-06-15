#ifndef GLASSPANEL_H
#define GLASSPANEL_H

#include <SFML/Graphics.hpp>

class GlassPanel {
public:
    static void draw(sf::RenderWindow& window, const sf::FloatRect& bounds, bool glowing = false, float outlineThickness = 1.0f);
};

#endif // GLASSPANEL_H
