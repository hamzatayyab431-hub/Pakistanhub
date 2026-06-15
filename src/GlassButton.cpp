#include "GlassButton.h"
#include "Theme.h"

GlassButton::GlassButton(const sf::Vector2f& pos, const sf::Vector2f& sz, sf::Font& fnt, const std::string& labelStr, Type type)
    : position(pos), size(sz), font(fnt), labelString(labelStr), isHovered(false), isEnabled(true), buttonType(type) {

    // Configure background rectangle
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);

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

void GlassButton::draw(sf::RenderWindow& window) {
    if (!isEnabled) {
        backgroundRect.setFillColor(sf::Color(100, 100, 100, 20));
        backgroundRect.setOutlineColor(sf::Color(100, 100, 100, 40));
        backgroundRect.setOutlineThickness(1.0f);
        labelText.setFillColor(sf::Color(150, 150, 150, 100));
        window.draw(backgroundRect);
        window.draw(labelText);
        return;
    }

    if (buttonType == Type::PRIMARY) {
        backgroundRect.setFillColor(isHovered ? Theme::GREEN_DIM : Theme::GREEN_PRIMARY);
        backgroundRect.setOutlineThickness(0.0f);
        labelText.setFillColor(Theme::BG_DARK);
    } else if (buttonType == Type::NAV_ACTIVE) {
        backgroundRect.setFillColor(Theme::GREEN_PRIMARY);
        backgroundRect.setOutlineThickness(0.0f);
        labelText.setFillColor(Theme::BG_DARK);
    } else {
        // NAV_DEFAULT
        if (isHovered) {
            backgroundRect.setFillColor(Theme::GLASS_FILL);
            backgroundRect.setOutlineColor(Theme::GREEN_PRIMARY);
            backgroundRect.setOutlineThickness(1.5f);
            labelText.setFillColor(Theme::TEXT_PRIMARY);
        } else {
            backgroundRect.setFillColor(sf::Color::Transparent);
            backgroundRect.setOutlineColor(Theme::GLASS_BORDER);
            backgroundRect.setOutlineThickness(1.0f);
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
