#include "Sidebar.h"
#include <cctype>

void Sidebar::init(sf::Font& fnt, SidebarItem active,
                   const std::string& displayName, const std::string& username)
{
    font = &fnt;

    // Logo — "P" icon + "akistanHub"
    logoP.setFont(fnt);
    logoP.setCharacterSize(26);
    logoP.setStyle(sf::Text::Bold);
    logoP.setFillColor(Theme::GREEN);
    logoP.setString("P");
    logoP.setPosition(18.f, 18.f);

    logoText.setFont(fnt);
    logoText.setCharacterSize(20);
    logoText.setStyle(sf::Text::Bold);
    logoText.setFillColor(Theme::TEXT_WHITE);
    logoText.setString("akistanHub");
    float pw = logoP.getGlobalBounds().width + 2.f;
    logoText.setPosition(18.f + pw, 22.f);

    // Nav items definition
    struct Def { SidebarItem id; const char* lbl; };
    Def defs[NAV_COUNT] = {
        { SidebarItem::HOME,    "Home"    },
        { SidebarItem::EXPLORE, "Explore" },
        { SidebarItem::PROFILE, "Profile" },
        { SidebarItem::LOGOUT,  "Logout"  },
    };

    for (int i = 0; i < NAV_COUNT; ++i) {
        auto& item = items[i];
        item.id       = defs[i].id;
        item.label    = defs[i].lbl;
        item.isActive = (defs[i].id == active);
        item.hoverAlpha.speed = 12.f;
        item.hoverAlpha.snap(0.f);

        item.text.setFont(fnt);
        item.text.setCharacterSize(15);
        item.text.setString(item.label);
        if (item.isActive) item.text.setStyle(sf::Text::Bold);
    }
    layoutItems();

    // User card background
    userCardBg.setSize(sf::Vector2f(Theme::SIDEBAR_W - 16.f, 52.f));
    userCardBg.setPosition(8.f, 720.f - 60.f);
    userCardBg.setFillColor(Theme::BG_ELEVATED);
    userCardBg.setOutlineColor(Theme::DIVIDER);
    userCardBg.setOutlineThickness(1.f);

    setUsername(username, displayName);
}

void Sidebar::layoutItems() {
    float startY = 80.f;
    for (int i = 0; i < NAV_COUNT; ++i) {
        auto& item = items[i];
        item.y = startY + i * (NavItem::H + 4.f);
        item.bg.setPosition(NavItem::X, item.y);
        item.bg.setSize(sf::Vector2f(NavItem::W, NavItem::H));
        sf::FloatRect tb = item.text.getLocalBounds();
        item.text.setOrigin(0.f, tb.top + tb.height / 2.f);
        item.text.setPosition(NavItem::X + 16.f, item.y + NavItem::H / 2.f);
    }
}

void Sidebar::setUsername(const std::string& username, const std::string& displayName) {
    if (!font) return;

    userNameText.setFont(*font);
    userNameText.setCharacterSize(13);
    userNameText.setStyle(sf::Text::Bold);
    userNameText.setFillColor(Theme::TEXT_WHITE);
    userNameText.setString(displayName.empty() ? username : displayName);
    userNameText.setPosition(56.f, 720.f - 55.f);

    userHandleText.setFont(*font);
    userHandleText.setCharacterSize(11);
    userHandleText.setFillColor(Theme::TEXT_DIM);
    userHandleText.setString("@" + username);
    userHandleText.setPosition(56.f, 720.f - 38.f);

    avatarCenter = sf::Vector2f(30.f, 720.f - 34.f);
    avatarInitial = (!displayName.empty() ? displayName[0] : (!username.empty() ? username[0] : '?'));
}

void Sidebar::draw(sf::RenderWindow& window) {
    DrawUtils::drawSidebar(window);

    window.draw(logoP);
    window.draw(logoText);

    for (int i = 0; i < NAV_COUNT; ++i) {
        auto& item = items[i];
        bool isLogout = (item.id == SidebarItem::LOGOUT);

        if (item.isActive) {
            // Filled active background
            sf::Color activeFill = Theme::GREEN_SUBTLE;
            activeFill.a = 40;
            DrawUtils::drawRoundRect(window,
                sf::FloatRect(NavItem::X, item.y, NavItem::W, NavItem::H),
                12.f, activeFill);
            // Left accent bar
            sf::RectangleShape accent(sf::Vector2f(3.f, NavItem::H - 12.f));
            accent.setPosition(NavItem::X, item.y + 6.f);
            accent.setFillColor(Theme::GREEN);
            window.draw(accent);
            item.text.setFillColor(Theme::GREEN);
        } else {
            // Hover fill
            sf::Uint8 ha = static_cast<sf::Uint8>(item.hoverAlpha.current);
            if (ha > 0) {
                sf::Color hf = isLogout ? sf::Color(239, 68, 68, ha / 4)
                                        : sf::Color(255, 255, 255, ha / 6);
                DrawUtils::drawRoundRect(window,
                    sf::FloatRect(NavItem::X, item.y, NavItem::W, NavItem::H),
                    12.f, hf);
            }
            sf::Color tc = isLogout
                ? (item.isHovered ? Theme::ERROR : Theme::TEXT_DIM)
                : (item.isHovered ? Theme::TEXT_WHITE : Theme::TEXT_MUTED);
            item.text.setFillColor(tc);
        }
        window.draw(item.text);
    }

    // User card
    DrawUtils::drawRoundRect(window,
        sf::FloatRect(8.f, 720.f - 60.f, Theme::SIDEBAR_W - 16.f, 52.f),
        10.f, Theme::BG_ELEVATED, Theme::DIVIDER, 1.f);

    if (avatarInitial != '?' && font) {
        DrawUtils::drawAvatar(window, avatarCenter, 16.f, avatarInitial, *font, 12);
    }
    window.draw(userNameText);
    window.draw(userHandleText);
}

void Sidebar::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        for (auto& item : items) {
            bool prev = item.isHovered;
            item.isHovered = hitTest(item, mp);
            if (item.isHovered != prev)
                item.hoverAlpha.setTarget(item.isHovered ? 255.f : 0.f);
        }
    }
}

void Sidebar::update(float dt) {
    for (auto& item : items) item.hoverAlpha.update(dt);
}

bool Sidebar::isClicked(sf::Event& event, SidebarItem id) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        for (auto& item : items) {
            if (item.id == id && hitTest(item, mp)) return true;
        }
    }
    return false;
}

bool Sidebar::hitTest(const NavItem& item, sf::Vector2f mp) const {
    return sf::FloatRect(NavItem::X, item.y, NavItem::W, NavItem::H).contains(mp);
}
