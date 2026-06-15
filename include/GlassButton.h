#ifndef GLASSBUTTON_H
#define GLASSBUTTON_H

#include "UIComponent.h"
#include <string>

class GlassButton : public UIComponent {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Font& font;
    std::string labelString;
    bool isHovered;

    sf::RectangleShape backgroundRect;
    sf::RectangleShape shadowRect; // Green glow on hover/active
    sf::Text labelText;

public:
    GlassButton(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& labelStr);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    bool isClicked(sf::Event& event, sf::RenderWindow& window);
    void setPosition(const sf::Vector2f& pos);
};

#endif // GLASSBUTTON_H
