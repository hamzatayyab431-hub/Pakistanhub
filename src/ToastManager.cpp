#include "ToastManager.h"
#include "DrawUtils.h"
#include <algorithm>

void ToastManager::push(const std::string& title,
                        const std::string& subtitle,
                        ToastType type)
{
    Toast t;
    t.title    = title;
    t.subtitle = subtitle;
    t.type     = type;
    t.xLerp.speed = 12.f;
    t.xLerp.snap(CARD_X_OUT);
    t.xLerp.setTarget(CARD_X_IN);
    t.alphaLerp.speed = 12.f;
    t.alphaLerp.snap(0.f);
    t.alphaLerp.setTarget(1.f);
    toasts.push_back(std::move(t));
}

void ToastManager::update(float dt) {
    for (auto& t : toasts) {
        t.xLerp.update(dt);
        t.alphaLerp.update(dt);

        if (!t.slidingOut) {
            t.elapsed += dt;
            if (t.elapsed >= t.lifetime) {
                t.slidingOut = true;
                t.xLerp.setTarget(CARD_X_OUT);
                t.alphaLerp.setTarget(0.f);
            }
        }
    }

    // Remove toasts that have fully slid out
    toasts.erase(std::remove_if(toasts.begin(), toasts.end(), [&](const Toast& t) {
        return t.slidingOut && t.xLerp.current > CARD_X_OUT - 5.f;
    }), toasts.end());
}

void ToastManager::draw(sf::RenderWindow& window, sf::Font& font) {
    float baseY = BOTTOM_Y;
    for (int i = static_cast<int>(toasts.size()) - 1; i >= 0; --i) {
        const Toast& t = toasts[i];
        float x = t.xLerp.current;
        float y = baseY - CARD_H;
        float alpha = t.alphaLerp.current;

        // Panel
        sf::Color fill = sf::Color(
            Theme::GLASS_3.r, Theme::GLASS_3.g, Theme::GLASS_3.b,
            static_cast<sf::Uint8>(Theme::GLASS_3.a * alpha));
        DrawUtils::drawGlassPanel(window,
            sf::FloatRect(x, y, CARD_W, CARD_H), 8.f, fill, false);

        // Left accent bar (4px)
        sf::Color bar = barColor(t.type);
        bar.a = static_cast<sf::Uint8>(255.f * alpha);
        sf::RectangleShape accentBar(sf::Vector2f(4.f, CARD_H - 16.f));
        accentBar.setPosition(x + 8.f, y + 8.f);
        accentBar.setFillColor(bar);
        window.draw(accentBar);

        // Icon indicator (simple shapes)
        {
            float ix = x + 22.f;
            float iy = y + CARD_H / 2.f;
            sf::Color iconCol = bar;
            if (t.type == ToastType::SUCCESS) {
                // checkmark: two lines
                sf::RectangleShape l1(sf::Vector2f(5.f, 1.5f));
                l1.setPosition(ix - 4.f, iy + 1.f);
                l1.setRotation(45.f);
                l1.setFillColor(iconCol);
                window.draw(l1);
                sf::RectangleShape l2(sf::Vector2f(9.f, 1.5f));
                l2.setPosition(ix - 2.f, iy + 4.f);
                l2.setRotation(-50.f);
                l2.setFillColor(iconCol);
                window.draw(l2);
            } else if (t.type == ToastType::ERROR) {
                // X mark
                sf::RectangleShape l1(sf::Vector2f(10.f, 1.5f));
                l1.setOrigin(5.f, 0.75f);
                l1.setPosition(ix, iy);
                l1.setRotation(45.f);
                l1.setFillColor(iconCol);
                window.draw(l1);
                sf::RectangleShape l2(sf::Vector2f(10.f, 1.5f));
                l2.setOrigin(5.f, 0.75f);
                l2.setPosition(ix, iy);
                l2.setRotation(-45.f);
                l2.setFillColor(iconCol);
                window.draw(l2);
            } else {
                // Info dot
                sf::CircleShape dot(3.f);
                dot.setOrigin(3.f, 3.f);
                dot.setPosition(ix, iy);
                dot.setFillColor(iconCol);
                window.draw(dot);
            }
        }

        // Title text
        sf::Text titleTxt;
        titleTxt.setFont(font);
        titleTxt.setCharacterSize(13);
        titleTxt.setStyle(sf::Text::Bold);
        sf::Color tc = Theme::TEXT_WHITE;
        tc.a = static_cast<sf::Uint8>(255.f * alpha);
        titleTxt.setFillColor(tc);
        titleTxt.setString(t.title);
        titleTxt.setPosition(x + 36.f, y + (t.subtitle.empty() ? 20.f : 12.f));
        window.draw(titleTxt);

        // Subtitle text
        if (!t.subtitle.empty()) {
            sf::Text subTxt;
            subTxt.setFont(font);
            subTxt.setCharacterSize(12);
            sf::Color sc = Theme::TEXT_MUTED;
            sc.a = static_cast<sf::Uint8>(255.f * alpha);
            subTxt.setFillColor(sc);
            subTxt.setString(t.subtitle);
            subTxt.setPosition(x + 36.f, y + 30.f);
            window.draw(subTxt);
        }

        baseY = y - STACK_GAP;
    }
}

sf::Color ToastManager::barColor(ToastType t) const {
    switch (t) {
        case ToastType::SUCCESS: return Theme::GREEN;
        case ToastType::ERROR:   return Theme::ERROR;
        case ToastType::WARNING: return Theme::WARNING;
        default:                 return Theme::GLASS_BORDER;
    }
}
