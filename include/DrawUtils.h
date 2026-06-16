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
        if (borderThick > 0.f) { r.setOutlineColor(border); r.setOutlineThickness(borderThick); }
        target.draw(r);
        return;
    }
    float x = rect.left, y = rect.top, w = rect.width, h = rect.height;

    sf::RectangleShape centre(sf::Vector2f(w, h - radius * 2.f));
    centre.setPosition(x, y + radius); centre.setFillColor(fill); target.draw(centre);
    sf::RectangleShape top(sf::Vector2f(w - radius * 2.f, radius));
    top.setPosition(x + radius, y); top.setFillColor(fill); target.draw(top);
    sf::RectangleShape bottom(sf::Vector2f(w - radius * 2.f, radius));
    bottom.setPosition(x + radius, y + h - radius); bottom.setFillColor(fill); target.draw(bottom);

    struct Corner { float cx, cy; };
    Corner corners[4] = {
        { x + radius, y + radius }, { x + w - radius, y + radius },
        { x + w - radius, y + h - radius }, { x + radius, y + h - radius }
    };
    for (auto& c : corners) {
        sf::CircleShape circle(radius);
        circle.setOrigin(radius, radius);
        circle.setPosition(c.cx, c.cy);
        circle.setFillColor(fill);
        target.draw(circle);
    }

    if (borderThick > 0.f && border.a > 0) {
        sf::RectangleShape bTop(sf::Vector2f(w - radius * 2.f, borderThick));
        bTop.setPosition(x + radius, y); bTop.setFillColor(border); target.draw(bTop);
        sf::RectangleShape bBot(sf::Vector2f(w - radius * 2.f, borderThick));
        bBot.setPosition(x + radius, y + h - borderThick); bBot.setFillColor(border); target.draw(bBot);
        sf::RectangleShape bLeft(sf::Vector2f(borderThick, h - radius * 2.f));
        bLeft.setPosition(x, y + radius); bLeft.setFillColor(border); target.draw(bLeft);
        sf::RectangleShape bRight(sf::Vector2f(borderThick, h - radius * 2.f));
        bRight.setPosition(x + w - borderThick, y + radius); bRight.setFillColor(border); target.draw(bRight);
        for (auto& c : corners) {
            sf::CircleShape arcFill(radius);
            arcFill.setOrigin(radius, radius);
            arcFill.setPosition(c.cx, c.cy);
            arcFill.setFillColor(sf::Color::Transparent);
            arcFill.setOutlineColor(border);
            arcFill.setOutlineThickness(borderThick);
            target.draw(arcFill);
        }
    }
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
