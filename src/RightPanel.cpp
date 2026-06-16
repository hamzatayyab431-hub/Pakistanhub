#include "RightPanel.h"
#include "DrawUtils.h"
#include <algorithm>
#include <cctype>

void RightPanel::init(sf::Font& fnt, UserManager& um, SocialGraph& sg) {
    font = &fnt;
    userMgr = &um;
    socialGraph = &sg;

    trendingTitle.setFont(fnt);
    trendingTitle.setCharacterSize(15);
    trendingTitle.setStyle(sf::Text::Bold);
    trendingTitle.setFillColor(Theme::TEXT_WHITE);
    trendingTitle.setString("Trending in Pakistan");
    trendingTitle.setPosition(Theme::RIGHT_X + 16.f, 16.f);

    whoToFollowTitle.setFont(fnt);
    whoToFollowTitle.setCharacterSize(15);
    whoToFollowTitle.setStyle(sf::Text::Bold);
    whoToFollowTitle.setFillColor(Theme::TEXT_WHITE);
    whoToFollowTitle.setString("Who to follow");
    whoToFollowTitle.setPosition(Theme::RIGHT_X + 16.f, 240.f);

    // Static trend data
    trends = {
        {"#Cricket",         "34.5K posts"},
        {"#Lahore",          "18.3K posts"},
        {"#PakistanZindabad","15.7K posts"},
        {"#Islamabad",       "12.1K posts"},
        {"#Karachi",         "11.3K posts"},
    };
}

void RightPanel::setCurrentUser(const std::string& username) {
    currentUsername = username;
    buildSuggestions();
}

void RightPanel::buildSuggestions() {
    suggestions.clear();
    if (!userMgr) return;
    int count = 0;
    for (const auto& u : userMgr->getUsers()) {
        if (u.getUsername() == currentUsername) continue;
        if (count >= 3) break;
        SuggestedUser s;
        s.displayName = u.getDisplayName().empty() ? u.getUsername() : u.getDisplayName();
        s.username    = u.getUsername();
        s.initial     = s.displayName.empty() ? '?' : s.displayName[0];
        s.hoverAlpha.speed = 12.f;
        s.hoverAlpha.snap(0.f);
        suggestions.push_back(std::move(s));
        ++count;
    }
}

void RightPanel::draw(sf::RenderWindow& window) {
    DrawUtils::drawRightPanel(window);
    if (!font) return;

    float rx = Theme::RIGHT_X;

    // ── Trending section ─────────────────────────────────────────────────────
    window.draw(trendingTitle);

    for (int i = 0; i < static_cast<int>(trends.size()); ++i) {
        float ty = 44.f + i * 38.f;

        // Row hover bg (static for now)
        sf::RectangleShape rowBg(sf::Vector2f(Theme::RIGHT_W - 2.f, 34.f));
        rowBg.setPosition(rx + 1.f, ty);
        rowBg.setFillColor(sf::Color::Transparent);
        window.draw(rowBg);

        sf::Text tagTxt;
        tagTxt.setFont(*font);
        tagTxt.setCharacterSize(13);
        tagTxt.setStyle(sf::Text::Bold);
        tagTxt.setFillColor(Theme::TEXT_WHITE);
        tagTxt.setString(trends[i].tag);
        tagTxt.setPosition(rx + 16.f, ty + 2.f);
        window.draw(tagTxt);

        sf::Text countTxt;
        countTxt.setFont(*font);
        countTxt.setCharacterSize(11);
        countTxt.setFillColor(Theme::TEXT_DIM);
        countTxt.setString(trends[i].count);
        countTxt.setPosition(rx + 16.f, ty + 18.f);
        window.draw(countTxt);
    }

    DrawUtils::drawDivider(window, rx, 236.f, Theme::RIGHT_W);

    // ── Who to follow ────────────────────────────────────────────────────────
    window.draw(whoToFollowTitle);

    for (int i = 0; i < static_cast<int>(suggestions.size()); ++i) {
        auto& s = suggestions[i];
        float sy = 268.f + i * 58.f;
        float ax = rx + 28.f;

        // Hover bg
        sf::Uint8 ha = static_cast<sf::Uint8>(s.hoverAlpha.current / 8.f);
        if (ha > 0) {
            sf::RectangleShape hbg(sf::Vector2f(Theme::RIGHT_W - 2.f, 50.f));
            hbg.setPosition(rx + 1.f, sy);
            hbg.setFillColor(sf::Color(255, 255, 255, ha));
            window.draw(hbg);
        }

        // Avatar
        DrawUtils::drawAvatar(window, sf::Vector2f(ax, sy + 25.f), 18.f, s.initial, *font, 13);

        // Name
        sf::Text nameTxt;
        nameTxt.setFont(*font);
        nameTxt.setCharacterSize(13);
        nameTxt.setStyle(sf::Text::Bold);
        nameTxt.setFillColor(Theme::TEXT_WHITE);
        nameTxt.setString(s.displayName);
        nameTxt.setPosition(ax + 24.f, sy + 8.f);
        window.draw(nameTxt);

        sf::Text handleTxt;
        handleTxt.setFont(*font);
        handleTxt.setCharacterSize(11);
        handleTxt.setFillColor(Theme::TEXT_DIM);
        handleTxt.setString("@" + s.username);
        handleTxt.setPosition(ax + 24.f, sy + 26.f);
        window.draw(handleTxt);

        // Follow pill button
        float bx = rx + Theme::RIGHT_W - 72.f;
        sf::Color btnFill = s.isHovered ? Theme::GREEN_DARK : Theme::GREEN;
        DrawUtils::drawRoundRect(window,
            sf::FloatRect(bx, sy + 12.f, 62.f, 24.f), 12.f, btnFill);
        sf::Text btnTxt;
        btnTxt.setFont(*font);
        btnTxt.setCharacterSize(11);
        btnTxt.setStyle(sf::Text::Bold);
        btnTxt.setFillColor(sf::Color::Black);
        btnTxt.setString("Follow");
        sf::FloatRect tb = btnTxt.getLocalBounds();
        btnTxt.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        btnTxt.setPosition(bx + 31.f, sy + 24.f);
        window.draw(btnTxt);
    }
}

void RightPanel::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        for (int i = 0; i < static_cast<int>(suggestions.size()); ++i) {
            auto& s = suggestions[i];
            float sy = 268.f + i * 58.f;
            bool prev = s.isHovered;
            s.isHovered = sf::FloatRect(Theme::RIGHT_X, sy, Theme::RIGHT_W, 50.f).contains(mp);
            if (s.isHovered != prev)
                s.hoverAlpha.setTarget(s.isHovered ? 255.f : 0.f);
        }
    }
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        for (int i = 0; i < static_cast<int>(suggestions.size()); ++i) {
            float sy = 268.f + i * 58.f;
            if (sf::FloatRect(Theme::RIGHT_X, sy, Theme::RIGHT_W, 50.f).contains(mp)) {
                clickedHandle = suggestions[i].username;
            }
        }
    }
}

void RightPanel::update(float dt) {
    for (auto& s : suggestions) s.hoverAlpha.update(dt);
}

std::string RightPanel::getClickedHandle()   { return clickedHandle; }
void        RightPanel::clearClickedHandle() { clickedHandle = ""; }
