#include "GlassButton.h"
#include "DrawUtils.h"
#include "Theme.h"

GlassButton::GlassButton(const sf::Vector2f& pos,
                         const sf::Vector2f& sz,
                         sf::Font&           fnt,
                         const std::string&  labelStr,
                         Type                type)
    : position(pos), size(sz), font(fnt), labelString(labelStr),
      isHovered(false), isPressed(false), isEnabled(true), buttonType(type)
{
    // Label text
    unsigned int charSize = static_cast<unsigned int>(sz.y * 0.38f);
    if (charSize < 11) charSize = 11;
    if (charSize > 16) charSize = 16;

    labelText.setFont(font);
    labelText.setCharacterSize(charSize);
    labelText.setString(labelString);

    if (type == Type::PRIMARY) {
        labelText.setStyle(sf::Text::Bold);
    }

    // Initial color snap (no animation on first frame)
    fillAnim.speed = 10.f;
    textAnim.speed = 10.f;
    updateColors();
    fillAnim.snap(fillAnim.target);
    textAnim.snap(textAnim.target);

    setPosition(pos);
}

void GlassButton::updateColors() {
    if (!isEnabled) {
        fillAnim.setTarget(sf::Color(20, 28, 38, 200));
        textAnim.setTarget(Theme::TEXT_DIM);
        return;
    }

    switch (buttonType) {
        case Type::PRIMARY:
            fillAnim.setTarget(isHovered ? Theme::GREEN_DARK : Theme::GREEN);
            textAnim.setTarget(Theme::BG_BASE);
            break;

        case Type::NAV_ACTIVE:
            fillAnim.setTarget(Theme::GREEN);
            textAnim.setTarget(Theme::BG_BASE);
            break;

        case Type::NAV_DEFAULT:
            fillAnim.setTarget(isHovered ? Theme::GLASS_2 : sf::Color(0, 0, 0, 0));
            textAnim.setTarget(isHovered ? Theme::TEXT_WHITE : Theme::TEXT_MUTED);
            break;

        case Type::LOGOUT:
            fillAnim.setTarget(isHovered ? sf::Color(180, 40, 40, 180) : sf::Color(0, 0, 0, 0));
            textAnim.setTarget(isHovered ? Theme::TEXT_WHITE : Theme::TEXT_MUTED);
            break;
    }
}

void GlassButton::update(float dt) {
    fillAnim.update(dt);
    textAnim.update(dt);
}

void GlassButton::draw(sf::RenderWindow& window) {
    float radius = std::min(size.x, size.y) * 0.45f;
    if (radius > 18.f) radius = 18.f;

    sf::Color fill = fillAnim.get();

    // For PRIMARY and NAV_ACTIVE, draw solid rounded pill
    if (fill.a > 0) {
        sf::Color border = (buttonType == Type::NAV_DEFAULT || buttonType == Type::LOGOUT)
                           ? Theme::GLASS_BORDER
                           : sf::Color::Transparent;
        DrawUtils::drawRoundRect(window,
            sf::FloatRect(position.x, position.y, size.x, size.y),
            radius, fill, border, 1.f);
    } else {
        // Transparent fill — draw just the border outline
        DrawUtils::drawRoundRect(window,
            sf::FloatRect(position.x, position.y, size.x, size.y),
            radius, sf::Color::Transparent, Theme::GLASS_BORDER, 1.f);
    }

    // Scale down slightly when pressed
    sf::Vector2f center(position.x + size.x / 2.f, position.y + size.y / 2.f);
    sf::Color tc = textAnim.get();
    if (!isEnabled) tc = Theme::TEXT_DIM;
    labelText.setFillColor(tc);

    sf::FloatRect tb = labelText.getLocalBounds();
    labelText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    if (isPressed && isEnabled) {
        labelText.setPosition(center.x, center.y + 1.f);
    } else {
        labelText.setPosition(center.x, center.y - 1.f);
    }
    window.draw(labelText);
}

void GlassButton::handleEvent(sf::Event& event) {
    if (!isEnabled) { isHovered = false; return; }

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        bool prev = isHovered;
        isHovered = sf::FloatRect(position.x, position.y, size.x, size.y).contains(mp);
        if (prev != isHovered) updateColors();
    }
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        if (sf::FloatRect(position.x, position.y, size.x, size.y).contains(mp))
            isPressed = true;
    }
    if (event.type == sf::Event::MouseButtonReleased) {
        isPressed = false;
    }
}

bool GlassButton::isClicked(sf::Event& event) {
    if (!isEnabled) return false;
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        return sf::FloatRect(position.x, position.y, size.x, size.y).contains(mp);
    }
    return false;
}

void GlassButton::setPosition(const sf::Vector2f& pos) {
    position = pos;
    sf::FloatRect tb = labelText.getLocalBounds();
    labelText.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    labelText.setPosition(pos.x + size.x / 2.f, pos.y + size.y / 2.f - 1.f);
}

void GlassButton::setEnabled(bool enabled) {
    isEnabled = enabled;
    updateColors();
}

bool GlassButton::getEnabled() const { return isEnabled; }

void GlassButton::setType(Type type) {
    buttonType = type;
    labelText.setStyle(type == Type::PRIMARY ? sf::Text::Bold : sf::Text::Regular);
    updateColors();
}

sf::FloatRect GlassButton::getBounds() const {
    return sf::FloatRect(position.x, position.y, size.x, size.y);
}
