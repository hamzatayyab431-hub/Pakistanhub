#include "TextInput.h"

TextInput::TextInput(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& placeholderStr, bool isPass)
    : position(pos), size(sz), font(fnt), placeholder(placeholderStr), isFocused(false), isPassword(isPass), isHovered(false) {

    // Configure main background rectangle
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);
    backgroundRect.setFillColor(sf::Color(255, 255, 255, 30)); // 30 alpha white
    backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 60)); // 60 alpha white
    backgroundRect.setOutlineThickness(1.0f);

    // Configure green shadow glow
    shadowRect.setPosition(position - sf::Vector2f(2, 2));
    shadowRect.setSize(size + sf::Vector2f(4, 4));
    shadowRect.setFillColor(sf::Color(0, 166, 81, 40)); // Pakistan Green #00A651 with 15% alpha
    shadowRect.setOutlineThickness(0);

    // Set up text formatting
    unsigned int charSize = static_cast<unsigned int>(size.y * 0.42f);
    
    displayedText.setFont(font);
    displayedText.setCharacterSize(charSize);
    displayedText.setFillColor(sf::Color::White);
    displayedText.setPosition(position.x + 12.0f, position.y + (size.y - charSize) / 2.0f - 4.0f);

    placeholderText.setFont(font);
    placeholderText.setCharacterSize(charSize);
    placeholderText.setFillColor(sf::Color(255, 255, 255, 110)); // 43% alpha white
    placeholderText.setPosition(position.x + 12.0f, position.y + (size.y - charSize) / 2.0f - 4.0f);
    placeholderText.setString(placeholder);
}

void TextInput::updateDisplayedText() {
    std::string showStr = textString;
    if (isPassword) {
        showStr = std::string(textString.length(), '*');
    }
    if (isFocused) {
        showStr += "_";
    }
    displayedText.setString(showStr);
}

void TextInput::draw(sf::RenderWindow& window) {
    if (isFocused) {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 50));
        backgroundRect.setOutlineColor(sf::Color(0, 166, 81, 200));
        window.draw(shadowRect);
    } else if (isHovered) {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 50));
        backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 120));
    } else {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 30));
        backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 60));
    }

    window.draw(backgroundRect);

    // If input is empty and not focused, show placeholder
    if (textString.empty() && !isFocused) {
        window.draw(placeholderText);
    } else {
        // Ensure visual cursor matches focus state
        updateDisplayedText();
        window.draw(displayedText);
    }
}

void TextInput::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isHovered = backgroundRect.getGlobalBounds().contains(mousePos);
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (backgroundRect.getGlobalBounds().contains(mousePos)) {
            isFocused = true;
        } else {
            isFocused = false;
        }
        updateDisplayedText();
    }

    if (isFocused && event.type == sf::Event::TextEntered) {
        sf::Uint32 unicode = event.text.unicode;
        if (unicode == 8) { // Backspace
            if (!textString.empty()) {
                textString.pop_back();
            }
        } else if (unicode >= 32 && unicode <= 126 && unicode != '|') {
            // Reject pipe delimiter and allow standard ASCII keys
            textString += static_cast<char>(unicode);
        }
        updateDisplayedText();
    }
}

std::string TextInput::getText() const {
    return textString;
}

void TextInput::clear() {
    textString.clear();
    updateDisplayedText();
}

void TextInput::setFocus(bool focus) {
    isFocused = focus;
    updateDisplayedText();
}

bool TextInput::getFocus() const {
    return isFocused;
}
