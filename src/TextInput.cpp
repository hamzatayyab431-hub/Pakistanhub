#include "TextInput.h"
#include "DrawUtils.h"
#include "Theme.h"

TextInput::TextInput(const sf::Vector2f& pos,
                     const sf::Vector2f& sz,
                     sf::Font&           fnt,
                     const std::string&  placeholderStr,
                     bool                isPass,
                     size_t              maxLen)
    : position(pos), size(sz), font(fnt),
      placeholder(placeholderStr),
      isFocused(false), isPassword(isPass), isHovered(false),
      maxLength(maxLen)
{
    unsigned int charSize = static_cast<unsigned int>(sz.y * 0.42f);
    if (charSize < 12) charSize = 12;
    if (charSize > 18) charSize = 18;

    displayedText.setFont(font);
    displayedText.setCharacterSize(charSize);
    displayedText.setFillColor(Theme::TEXT_WHITE);
    displayedText.setPosition(pos.x + 16.f, pos.y + (sz.y - charSize) / 2.f - 4.f);

    placeholderText.setFont(font);
    placeholderText.setCharacterSize(charSize);
    placeholderText.setFillColor(Theme::TEXT_DIM);
    placeholderText.setPosition(pos.x + 16.f, pos.y + (sz.y - charSize) / 2.f - 4.f);
    placeholderText.setString(placeholder);

    // Animated border colour
    borderAnim.speed = 10.f;
    borderAnim.snap(Theme::GLASS_BORDER);
    borderAnim.setTarget(Theme::GLASS_BORDER);
}

void TextInput::updateDisplayedText() {
    std::string s = textString;
    if (isPassword) s = std::string(s.length(), '*');
    displayedText.setString(s);
}

void TextInput::draw(sf::RenderWindow& window) {
    sf::Color border = borderAnim.get();

    // Inset dark fill so inputs read as "sunken"
    sf::Color fill(0, 0, 0, 60);

    // Glass panel base
    DrawUtils::drawRoundRect(window,
        sf::FloatRect(position.x, position.y, size.x, size.y),
        6.f, fill, border, 1.f);

    // Focus glow — slightly larger rect behind
    if (isFocused) {
        sf::Color glow = Theme::GREEN_GLOW;
        glow.a = 30;
        DrawUtils::drawRoundRect(window,
            sf::FloatRect(position.x - 3.f, position.y - 3.f, size.x + 6.f, size.y + 6.f),
            9.f, glow);
    }

    // Text
    if (textString.empty() && !isFocused) {
        window.draw(placeholderText);
    } else {
        std::string show = textString;
        if (isPassword) show = std::string(show.length(), '*');

        // Animated blinking cursor
        if (isFocused) {
            if (blink.tick()) show += "|";
        }
        displayedText.setString(show);
        window.draw(displayedText);
    }
}

void TextInput::update(float dt) {
    borderAnim.update(dt);
}

void TextInput::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        isHovered = sf::FloatRect(position.x, position.y, size.x, size.y).contains(mp);
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        bool wasF = isFocused;
        isFocused = sf::FloatRect(position.x, position.y, size.x, size.y).contains(mp);
        if (isFocused && !wasF) blink = BlinkClock{};
        borderAnim.setTarget(isFocused ? Theme::GREEN : Theme::GLASS_BORDER);
        updateDisplayedText();
    }

    if (isFocused && event.type == sf::Event::TextEntered) {
        sf::Uint32 uni = event.text.unicode;
        if (uni == 8) {
            if (!textString.empty()) textString.pop_back();
        } else if (uni >= 32 && uni <= 126 && uni != '|') {
            if (textString.length() < maxLength) {
                textString += static_cast<char>(uni);
            }
        }
        updateDisplayedText();
    }
}

std::string TextInput::getText() const { return textString; }

void TextInput::setText(const std::string& text) {
    textString = text;
    updateDisplayedText();
}

void TextInput::clear() {
    textString.clear();
    updateDisplayedText();
}

void TextInput::setFocus(bool focus) {
    isFocused = focus;
    borderAnim.setTarget(focus ? Theme::GREEN : Theme::GLASS_BORDER);
    updateDisplayedText();
}

bool TextInput::getFocus() const { return isFocused; }
