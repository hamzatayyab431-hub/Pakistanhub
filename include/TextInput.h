#ifndef TEXTINPUT_H
#define TEXTINPUT_H

#include "UIComponent.h"
#include "Animator.h"
#include <string>

class TextInput : public UIComponent {
private:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Font&    font;
    std::string  textString;
    std::string  placeholder;
    bool         isFocused;
    bool         isPassword;
    bool         isHovered;
    size_t       maxLength;

    sf::Text     displayedText;
    sf::Text     placeholderText;
    BlinkClock   blink;
    LerpColor    borderAnim;

    void updateDisplayedText();

public:
    TextInput(const sf::Vector2f& pos,
              const sf::Vector2f& sz,
              sf::Font&           fnt,
              const std::string&  placeholderStr = "",
              bool                isPass = false,
              size_t              maxLen = 280);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update(float dt = 0.016f);

    std::string getText() const;
    void setText(const std::string& text);
    void clear();
    void setFocus(bool focus);
    bool getFocus() const;
};

#endif // TEXTINPUT_H
