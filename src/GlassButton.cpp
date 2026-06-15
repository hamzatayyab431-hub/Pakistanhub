#include "GlassButton.h"
#include "Theme.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GlassButton::GlassButton(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& labelStr, Type type)
    : position(pos), size(sz), font(fnt), labelString(labelStr), isHovered(false), isEnabled(true), buttonType(type) {

    // Configure background rectangle
    backgroundRect.setPosition(position);
    updateShape();

    // Configure label text
    unsigned int charSize = static_cast<unsigned int>(size.y * 0.40f);
    labelText.setFont(font);
    labelText.setCharacterSize(charSize);
    labelText.setString(labelString);

    if (buttonType == Type::PRIMARY) {
        labelText.setStyle(sf::Text::Bold);
    }

    // Center the text origin inside the button bounds
    sf::FloatRect textRect = labelText.getLocalBounds();
    labelText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    labelText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f - 2.0f);
}

void GlassButton::updateShape() {
    float radius = std::min(size.x, size.y) * 0.15f; // Dynamic radius based on size
    if (radius > 10.0f) radius = 10.0f; // Max radius
    
    int cornerPoints = 8;
    backgroundRect.setPointCount(cornerPoints * 4);
    
    // Top-left
    for (int i = 0; i < cornerPoints; ++i) {
        float angle = M_PI + M_PI / 2.0f * i / (cornerPoints - 1);
        backgroundRect.setPoint(i, sf::Vector2f(radius + std::cos(angle) * radius, radius + std::sin(angle) * radius));
    }
    // Top-right
    for (int i = 0; i < cornerPoints; ++i) {
        float angle = 1.5f * M_PI + M_PI / 2.0f * i / (cornerPoints - 1);
        backgroundRect.setPoint(cornerPoints + i, sf::Vector2f(size.x - radius + std::cos(angle) * radius, radius + std::sin(angle) * radius));
    }
    // Bottom-right
    for (int i = 0; i < cornerPoints; ++i) {
        float angle = 0.0f + M_PI / 2.0f * i / (cornerPoints - 1);
        backgroundRect.setPoint(cornerPoints * 2 + i, sf::Vector2f(size.x - radius + std::cos(angle) * radius, size.y - radius + std::sin(angle) * radius));
    }
    // Bottom-left
    for (int i = 0; i < cornerPoints; ++i) {
        float angle = M_PI / 2.0f + M_PI / 2.0f * i / (cornerPoints - 1);
        backgroundRect.setPoint(cornerPoints * 3 + i, sf::Vector2f(radius + std::cos(angle) * radius, size.y - radius + std::sin(angle) * radius));
    }
}

void GlassButton::draw(sf::RenderWindow& window) {
    if (!isEnabled) {
        backgroundRect.setFillColor(Theme::BG_MID);
        backgroundRect.setOutlineThickness(0.0f);
        labelText.setFillColor(Theme::TEXT_DIM);
        window.draw(backgroundRect);
        window.draw(labelText);
        return;
    }

    if (buttonType == Type::PRIMARY) {
        backgroundRect.setFillColor(isHovered ? Theme::GREEN_PRIMARY : Theme::GREEN_DIM);
        backgroundRect.setOutlineThickness(0.0f);
        labelText.setFillColor(Theme::BG_DARK);
    } else if (buttonType == Type::NAV_ACTIVE) {
        backgroundRect.setFillColor(Theme::GREEN_PRIMARY);
        backgroundRect.setOutlineThickness(0.0f);
        labelText.setFillColor(Theme::BG_DARK);
    } else {
        // NAV_DEFAULT
        if (isHovered) {
            backgroundRect.setFillColor(sf::Color(45, 55, 70, 255)); // Professional slightly brightened solid fill
            backgroundRect.setOutlineThickness(0.0f);
            labelText.setFillColor(sf::Color::White);
        } else {
            backgroundRect.setFillColor(Theme::BG_MID); // Solid dark background
            backgroundRect.setOutlineThickness(0.0f);
            labelText.setFillColor(Theme::TEXT_MUTED);
        }
    }

    window.draw(backgroundRect);
    window.draw(labelText);
}

void GlassButton::handleEvent(sf::Event& event) {
    if (!isEnabled) {
        isHovered = false;
        return;
    }
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isHovered = backgroundRect.getGlobalBounds().contains(mousePos);
    }
}

bool GlassButton::isClicked(sf::Event& event) {
    if (!isEnabled) return false;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return backgroundRect.getGlobalBounds().contains(mousePos);
    }
    return false;
}

void GlassButton::setPosition(const sf::Vector2f& pos) {
    position = pos;
    backgroundRect.setPosition(position);
    
    sf::FloatRect textRect = labelText.getLocalBounds();
    labelText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f - 2.0f);
}

void GlassButton::setEnabled(bool enabled) {
    isEnabled = enabled;
}

bool GlassButton::getEnabled() const {
    return isEnabled;
}

void GlassButton::setType(Type type) {
    buttonType = type;
    if (buttonType == Type::PRIMARY) {
        labelText.setStyle(sf::Text::Bold);
    } else {
        labelText.setStyle(sf::Text::Regular);
    }
    // Re-center just in case font style changes bounds slightly
    sf::FloatRect textRect = labelText.getLocalBounds();
    labelText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    labelText.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f - 2.0f);
}
