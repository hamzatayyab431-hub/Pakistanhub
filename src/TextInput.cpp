#include "TextInput.h"
#include "Theme.h"
#include "GlassPanel.h"

TextInput::TextInput(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& placeholderStr, bool isPass, size_t maxLen)
    : position(pos), size(sz), font(fnt), placeholder(placeholderStr), isFocused(false), isPassword(isPass), isHovered(false), maxLength(maxLen) {

    // Set up text formatting
    unsigned int charSize = static_cast<unsigned int>(size.y * 0.42f);
    
    displayedText.setFont(font);
    displayedText.setCharacterSize(charSize);
    displayedText.setFillColor(Theme::TEXT_PRIMARY);
    displayedText.setPosition(position.x + 16.0f, position.y + (size.y - charSize) / 2.0f - 4.0f);

    placeholderText.setFont(font);
    placeholderText.setCharacterSize(charSize);
    placeholderText.setFillColor(Theme::TEXT_DIM);
    placeholderText.setPosition(position.x + 16.0f, position.y + (size.y - charSize) / 2.0f - 4.0f);
    placeholderText.setString(placeholder);
}

void TextInput::updateDisplayedText() {
    std::string showStr = textString;
    if (isPassword) {
        showStr = std::string(textString.length(), '*');
    }
    // Cursor is animated in draw() based on cursorClock
    displayedText.setString(showStr);
}

void TextInput::draw(sf::RenderWindow& window) {
    sf::Color outlineCol = Theme::GLASS_BORDER;
    if (isFocused) {
        outlineCol = Theme::GREEN_PRIMARY;
    }

    GlassPanel::draw(window, sf::FloatRect(position.x, position.y, size.x, size.y), isFocused, 1.0f, outlineCol);

    // If input is empty and not focused, show placeholder
    if (textString.empty() && !isFocused) {
        window.draw(placeholderText);
    } else {
        std::string showStr = textString;
        if (isPassword) {
            showStr = std::string(textString.length(), '*');
        }
        
        // Animated cursor
        if (isFocused) {
            if (static_cast<int>(cursorClock.getElapsedTime().asMilliseconds() / 500) % 2 == 0) {
                showStr += "|";
            }
        }
        displayedText.setString(showStr);
        window.draw(displayedText);
    }
}

void TextInput::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        sf::FloatRect bounds(position.x, position.y, size.x, size.y);
        isHovered = bounds.contains(mousePos);
    }

    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        sf::FloatRect bounds(position.x, position.y, size.x, size.y);
        if (bounds.contains(mousePos)) {
            if (!isFocused) cursorClock.restart();
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
            if (textString.length() < maxLength) {
                textString += static_cast<char>(unicode);
            }
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
