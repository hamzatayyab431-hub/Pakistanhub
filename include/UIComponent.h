#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include <SFML/Graphics.hpp>

class UIComponent {
public:
    virtual ~UIComponent() = default;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual void handleEvent(sf::Event& event) = 0;
};

#endif // UICOMPONENT_H
