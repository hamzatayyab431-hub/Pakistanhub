#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Theme.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace DrawUtils {

// ─── drawRoundRect ────────────────────────────────────────────────────────────
// Simulates border-radius using SFML primitives.
// Draws: 3 rects + 4 corner circles
inline void drawRoundRect(sf::RenderTarget& target,
                          sf::FloatRect      rect,
                          float              radius,
                          sf::Color          fill,
                          sf::Color          border    = sf::Color::Transparent,
                          float              borderThick = 0.f)
{
    if (radius <= 0.f || radius * 2.f > rect.width || radius * 2.f > rect.height) {
        // Fallback: plain rectangle
        sf::RectangleShape r(sf::Vector2f(rect.width, rect.height));
        r.setPosition(rect.left, rect.top);
        r.setFillColor(fill);
        if (borderThick > 0.f) {
            r.setOutlineColor(border);
            r.setOutlineThickness(borderThick);
        }
        target.draw(r);
        return;
    }

    float x = rect.left, y = rect.top, w = rect.width, h = rect.height;

    // Centre strip (full width, minus top/bottom radii)
    sf::RectangleShape centre(sf::Vector2f(w, h - radius * 2.f));
    centre.setPosition(x, y + radius);
    centre.setFillColor(fill);
    target.draw(centre);

    // Top strip (full width, radius height)
    sf::RectangleShape top(sf::Vector2f(w - radius * 2.f, radius));
    top.setPosition(x + radius, y);
    top.setFillColor(fill);
    target.draw(top);

    // Bottom strip
    sf::RectangleShape bottom(sf::Vector2f(w - radius * 2.f, radius));
    bottom.setPosition(x + radius, y + h - radius);
    bottom.setFillColor(fill);
    target.draw(bottom);

    // Four corner circles
    struct Corner { float cx, cy; };
    Corner corners[4] = {
        { x + radius,       y + radius       },   // TL
        { x + w - radius,   y + radius       },   // TR
        { x + w - radius,   y + h - radius   },   // BR
        { x + radius,       y + h - radius   }    // BL
    };
    for (auto& c : corners) {
        sf::CircleShape circle(radius);
        circle.setOrigin(radius, radius);
        circle.setPosition(c.cx, c.cy);
        circle.setFillColor(fill);
        target.draw(circle);
    }

    // Border (drawn as outlines on rectangles + arcs approximated by outline circles)
    if (borderThick > 0.f && border.a > 0) {
        // top/bottom horizontal edges
        sf::RectangleShape bTop(sf::Vector2f(w - radius * 2.f, borderThick));
        bTop.setPosition(x + radius, y);
        bTop.setFillColor(border);
        target.draw(bTop);

        sf::RectangleShape bBot(sf::Vector2f(w - radius * 2.f, borderThick));
        bBot.setPosition(x + radius, y + h - borderThick);
        bBot.setFillColor(border);
        target.draw(bBot);

        // left/right vertical edges
        sf::RectangleShape bLeft(sf::Vector2f(borderThick, h - radius * 2.f));
        bLeft.setPosition(x, y + radius);
        bLeft.setFillColor(border);
        target.draw(bLeft);

        sf::RectangleShape bRight(sf::Vector2f(borderThick, h - radius * 2.f));
        bRight.setPosition(x + w - borderThick, y + radius);
        bRight.setFillColor(border);
        target.draw(bRight);

        // Corner arcs using CircleShape with outline trick
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

// ─── drawGlassPanel ───────────────────────────────────────────────────────────
// Layered glass panel:  outer glow (optional) → fill → border → top highlight
inline void drawGlassPanel(sf::RenderTarget& target,
                            sf::FloatRect      bounds,
                            float              radius   = 8.f,
                            sf::Color          fill     = Theme::GLASS_1,
                            bool               glowing  = false,
                            sf::Color          glowCol  = Theme::GREEN_GLOW)
{
    // 1. Outer glow
    if (glowing && glowCol.a > 0) {
        drawRoundRect(target,
            sf::FloatRect(bounds.left - 6.f, bounds.top - 6.f,
                          bounds.width + 12.f, bounds.height + 12.f),
            radius + 6.f, glowCol);
    }

    // 2. Fill
    drawRoundRect(target, bounds, radius, fill, Theme::GLASS_BORDER, 1.f);

    // 3. Top highlight (1px)
    sf::RectangleShape highlight(sf::Vector2f(bounds.width - radius * 2.f, 1.f));
    highlight.setPosition(bounds.left + radius, bounds.top);
    highlight.setFillColor(Theme::GLASS_TOP);
    target.draw(highlight);
}

// ─── drawAvatar ───────────────────────────────────────────────────────────────
// Draws: outer glow ring → border ring → fill circle → initial letter
inline void drawAvatar(sf::RenderTarget& target,
                        sf::Vector2f       center,
                        float              radius,
                        char               initial,
                        sf::Font&          font,
                        unsigned int       charSize)
{
    // Glow ring
    sf::CircleShape glow(radius + 4.f);
    glow.setOrigin(radius + 4.f, radius + 4.f);
    glow.setPosition(center);
    glow.setFillColor(Theme::GREEN_GLOW);
    target.draw(glow);

    // Border ring
    sf::CircleShape border(radius + 1.5f);
    border.setOrigin(radius + 1.5f, radius + 1.5f);
    border.setPosition(center);
    border.setFillColor(Theme::GREEN);
    target.draw(border);

    // Fill
    sf::CircleShape fill(radius);
    fill.setOrigin(radius, radius);
    fill.setPosition(center);
    fill.setFillColor(sf::Color(30, 40, 50, 255));
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

} // namespace DrawUtils
