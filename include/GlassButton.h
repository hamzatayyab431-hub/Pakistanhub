#ifndef GLASSBUTTON_H
#define GLASSBUTTON_H

#include "UIComponent.h"
#include "Animator.h"
#include <string>

class GlassButton : public UIComponent {
public:
    enum class Type { PRIMARY, NAV_DEFAULT, NAV_ACTIVE, LOGOUT };

private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Font&    font;
    std::string  labelString;
    bool         isHovered;
    bool         isPressed;
    bool         isEnabled;
    Type         buttonType;

    // Animated fill colour
    LerpColor    fillAnim;
    LerpColor    textAnim;

    sf::Text     labelText;

    void updateColors();

public:
    GlassButton(const sf::Vector2f& pos,
                const sf::Vector2f& sz,
                sf::Font&           fnt,
                const std::string&  labelStr,
                Type                type = Type::PRIMARY);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update(float dt);

    bool isClicked(sf::Event& event);
    void setPosition(const sf::Vector2f& pos);
    void setEnabled(bool enabled);
    bool getEnabled() const;
    void setType(Type type);
    Type getType() const { return buttonType; }
    sf::FloatRect getBounds() const;
};

#endif // GLASSBUTTON_H
