#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cctype>
#include "Theme.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace DrawUtils {

// ─── drawRoundRect ────────────────────────────────────────────────────────────
inline void drawRoundRect(sf::RenderTarget& target,
                          sf::FloatRect      rect,
                          float              radius,
                          sf::Color          fill,
                          sf::Color          border    = sf::Color::Transparent,
                          float              borderThick = 0.f)
{
    if (radius <= 0.f || radius * 2.f > rect.width || radius * 2.f > rect.height) {
        sf::RectangleShape r(sf::Vector2f(rect.width, rect.height));
        r.setPosition(rect.left, rect.top);
        r.setFillColor(fill);
        if (borderThick > 0.f) { r.setOutlineColor(border); r.setOutlineThickness(-borderThick); }
        target.draw(r);
        return;
    }
    float x = rect.left, y = rect.top, w = rect.width, h = rect.height;

    const unsigned int cornerPoints = 15;
    sf::ConvexShape shape(cornerPoints * 4);

    sf::Vector2f cTopLeft(x + radius, y + radius);
    sf::Vector2f cTopRight(x + w - radius, y + radius);
    sf::Vector2f cBotRight(x + w - radius, y + h - radius);
    sf::Vector2f cBotLeft(x + radius, y + h - radius);

    unsigned int ptIdx = 0;

    for (unsigned int i = 0; i < cornerPoints; ++i) {
        float angle = M_PI + (M_PI / 2.f) * (static_cast<float>(i) / (cornerPoints - 1));
        shape.setPoint(ptIdx++, sf::Vector2f(cTopLeft.x + std::cos(angle) * radius, cTopLeft.y + std::sin(angle) * radius));
    }
    for (unsigned int i = 0; i < cornerPoints; ++i) {
        float angle = 1.5f * M_PI + (M_PI / 2.f) * (static_cast<float>(i) / (cornerPoints - 1));
        shape.setPoint(ptIdx++, sf::Vector2f(cTopRight.x + std::cos(angle) * radius, cTopRight.y + std::sin(angle) * radius));
    }
    for (unsigned int i = 0; i < cornerPoints; ++i) {
        float angle = 0.f + (M_PI / 2.f) * (static_cast<float>(i) / (cornerPoints - 1));
        shape.setPoint(ptIdx++, sf::Vector2f(cBotRight.x + std::cos(angle) * radius, cBotRight.y + std::sin(angle) * radius));
    }
    for (unsigned int i = 0; i < cornerPoints; ++i) {
        float angle = M_PI / 2.f + (M_PI / 2.f) * (static_cast<float>(i) / (cornerPoints - 1));
        shape.setPoint(ptIdx++, sf::Vector2f(cBotLeft.x + std::cos(angle) * radius, cBotLeft.y + std::sin(angle) * radius));
    }

    shape.setFillColor(fill);
    if (borderThick > 0.f && border.a > 0) {
        shape.setOutlineColor(border);
        shape.setOutlineThickness(-borderThick);
    }
    target.draw(shape);
}

// ─── drawCard ─────────────────────────────────────────────────────────────────
// Clean solid card — no glass, no glow; matches new design language
inline void drawCard(sf::RenderTarget& target,
                     sf::FloatRect      bounds,
                     float              radius  = 16.f,
                     sf::Color          fill    = Theme::CARD_BG,
                     sf::Color          border  = Theme::DIVIDER,
                     float              bThick  = 1.f)
{
    drawRoundRect(target, bounds, radius, fill, border, bThick);
}

// ─── drawSidebar ──────────────────────────────────────────────────────────────
inline void drawSidebar(sf::RenderTarget& target) {
    sf::RectangleShape sb(sf::Vector2f(Theme::SIDEBAR_W, 720.f));
    sb.setPosition(0.f, 0.f);
    sb.setFillColor(Theme::SIDEBAR_BG);
    target.draw(sb);
    // Right border
    sf::RectangleShape border(sf::Vector2f(1.f, 720.f));
    border.setPosition(Theme::SIDEBAR_W - 1.f, 0.f);
    border.setFillColor(Theme::DIVIDER);
    target.draw(border);
}

// ─── drawRightPanel ───────────────────────────────────────────────────────────
inline void drawRightPanel(sf::RenderTarget& target) {
    sf::RectangleShape rp(sf::Vector2f(Theme::RIGHT_W, 720.f));
    rp.setPosition(Theme::RIGHT_X, 0.f);
    rp.setFillColor(Theme::SIDEBAR_BG);
    target.draw(rp);
    sf::RectangleShape border(sf::Vector2f(1.f, 720.f));
    border.setPosition(Theme::RIGHT_X, 0.f);
    border.setFillColor(Theme::DIVIDER);
    target.draw(border);
}

// ─── drawGlassPanel (legacy compat) ──────────────────────────────────────────
inline void drawGlassPanel(sf::RenderTarget& target,
                            sf::FloatRect      bounds,
                            float              radius  = 8.f,
                            sf::Color          fill    = Theme::GLASS_1,
                            bool               /*glowing*/ = false,
                            sf::Color          /*glowCol*/  = Theme::GREEN_GLOW)
{
    drawRoundRect(target, bounds, radius, fill, Theme::GLASS_BORDER, 1.f);
}

// ─── drawAvatar ───────────────────────────────────────────────────────────────
inline void drawAvatar(sf::RenderTarget& target,
                        sf::Vector2f       center,
                        float              radius,
                        char               initial,
                        sf::Font&          font,
                        unsigned int       charSize)
{
    // Border ring — green
    sf::CircleShape border(radius + 1.5f);
    border.setOrigin(radius + 1.5f, radius + 1.5f);
    border.setPosition(center);
    border.setFillColor(Theme::GREEN);
    target.draw(border);

    // Fill
    sf::CircleShape fill(radius);
    fill.setOrigin(radius, radius);
    fill.setPosition(center);
    fill.setFillColor(sf::Color(20, 32, 50, 255));
    target.draw(fill);

    // Letter
    sf::Text letter;
    letter.setFont(font);
    letter.setCharacterSize(charSize);
    letter.setStyle(sf::Text::Bold);
    letter.setFillColor(Theme::TEXT_WHITE);
    letter.setString(std::string(1, static_cast<char>(std::toupper(initial))));
    sf::FloatRect lb = letter.getLocalBounds();
    letter.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    letter.setPosition(center);
    target.draw(letter);
}

// ─── drawDivider ──────────────────────────────────────────────────────────────
inline void drawDivider(sf::RenderTarget& target, float x, float y, float w) {
    sf::RectangleShape div(sf::Vector2f(w, 1.f));
    div.setPosition(x, y);
    div.setFillColor(Theme::DIVIDER);
    target.draw(div);
}

// ─── centreText ───────────────────────────────────────────────────────────────
inline void centreText(sf::Text& t, float cx) {
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, 0.f);
    t.setPosition(cx, t.getPosition().y);
}

} // namespace DrawUtils
