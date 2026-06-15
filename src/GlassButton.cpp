#include "GlassButton.h"

GlassButton::GlassButton(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& labelStr)
    : position(pos), size(sz), font(fnt), labelString(labelStr), isHovered(false) {

    // Configure background rectangle
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);
    backgroundRect.setFillColor(sf::Color(255, 255, 255, 30)); // 12% alpha white
    backgroundRect.setOutlineColor(sf::Color(0, 166, 81, 150)); // Pakistan Green with 58% alpha
    backgroundRect.setOutlineThickness(1.0f);

    // Configure glow shadow
    shadowRect.setPosition(position - sf::Vector2f(2, 2));
    shadowRect.setSize(size + sf::Vector2f(4, 4));
    shadowRect.setFillColor(sf::Color(0, 166, 81, 40)); // 15% alpha green
    shadowRect.setOutlineThickness(0);

    // Configure label text
    unsigned int charSize = static_cast<unsigned int>(size.y * 0.40f);
    labelText.setFont(font);
    labelText.setCharacterSize(charSize);
    labelText.setFillColor(sf::Color::White);
    labelText.setString(labelString);

    // Center the text origin inside the button bounds
    sf::FloatRect textRect = labelText.getLocalBounds();
    labelText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    labelText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}

void GlassButton::draw(sf::RenderWindow& window) {
    if (isHovered) {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 50)); // Lighten panel on hover
        backgroundRect.setOutlineColor(sf::Color(0, 166, 81, 255)); // Active green border
        window.draw(shadowRect); // Draw glowing green shadow
    } else {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 30));
        backgroundRect.setOutlineColor(sf::Color(0, 166, 81, 150));
    }

    window.draw(backgroundRect);
    window.draw(labelText);
}

void GlassButton::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isHovered = backgroundRect.getGlobalBounds().contains(mousePos);
    }
}

bool GlassButton::isClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return backgroundRect.getGlobalBounds().contains(mousePos);
    }
    return false;
}

void GlassButton::setPosition(const sf::Vector2f& pos) {
    position = pos;
    backgroundRect.setPosition(position);
    shadowRect.setPosition(position - sf::Vector2f(2, 2));

    sf::FloatRect textRect = labelText.getLocalBounds();
    labelText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
}
