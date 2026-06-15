#include "GlassPanel.h"
#include "Theme.h"

void GlassPanel::draw(sf::RenderWindow& window, const sf::FloatRect& bounds, bool glowing, float outlineThickness) {
    if (glowing) {
        sf::RectangleShape glowRect(sf::Vector2f(bounds.width + 6.0f, bounds.height + 6.0f));
        glowRect.setPosition(bounds.left - 3.0f, bounds.top - 3.0f);
        glowRect.setFillColor(Theme::GREEN_GLOW);
        window.draw(glowRect);
    }

    sf::RectangleShape fillRect(sf::Vector2f(bounds.width, bounds.height));
    fillRect.setPosition(bounds.left, bounds.top);
    fillRect.setFillColor(Theme::GLASS_FILL);
    
    if (outlineThickness > 0.0f) {
        fillRect.setOutlineColor(Theme::GLASS_BORDER);
        fillRect.setOutlineThickness(outlineThickness);
    }
    window.draw(fillRect);

    // Top highlight line for simulated light depth
    sf::RectangleShape highlight(sf::Vector2f(bounds.width, 1.0f));
    highlight.setPosition(bounds.left, bounds.top);
    highlight.setFillColor(sf::Color(255, 255, 255, 60));
    window.draw(highlight);
}
