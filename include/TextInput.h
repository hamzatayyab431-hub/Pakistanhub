#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "UIComponent.h"
#include <string>

class TextInput : public UIComponent {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Font& font;
    std::string textString;
    std::string placeholder;
    bool isFocused;
    bool isPassword;

    // UI Shapes
    sf::RectangleShape backgroundRect;
    sf::RectangleShape shadowRect; // Green glow behind input when focused
    sf::Text displayedText;
    sf::Text placeholderText;

    void updateDisplayedText();

public:
    TextInput(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& placeholderStr = "", bool isPass = false);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    std::string getText() const;
    void clear();
    void setFocus(bool focus);
    bool getFocus() const;
};

#endif // TEXTINPUT_H
