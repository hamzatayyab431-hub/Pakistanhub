#pragma once
#include <SFML/Graphics.hpp>
#include "Animator.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <string>
#include <array>

enum class SidebarItem { HOME, EXPLORE, PROFILE, LOGOUT, NONE };

// One clickable nav row in the sidebar
struct NavItem {
    SidebarItem id;
    std::string label;
    sf::Text    text;
    sf::RectangleShape bg;
    Lerp        hoverAlpha;
    bool        isHovered = false;
    bool        isActive  = false;
    float       y         = 0.f;

    static constexpr float H = 44.f;
    static constexpr float W = Theme::SIDEBAR_W - 16.f;
    static constexpr float X = 8.f;
};

class Sidebar {
public:
    Sidebar() = default;
    void init(sf::Font& font, SidebarItem active, const std::string& displayName,
              const std::string& username);

    void draw(sf::RenderWindow& window);
    void handleEvent(sf::Event& event);
    void update(float dt);

    bool isClicked(sf::Event& event, SidebarItem item);
    void setUsername(const std::string& username, const std::string& displayName);

private:
    sf::Font* font = nullptr;

    // Logo
    sf::Text logoP;
    sf::Text logoText;

    // Nav items
    static constexpr int NAV_COUNT = 4;
    NavItem items[NAV_COUNT];

    // Mini user card at bottom
    sf::RectangleShape userCardBg;
    sf::Text           userNameText;
    sf::Text           userHandleText;
    sf::Vector2f       avatarCenter;
    char               avatarInitial = '?';

    void layoutItems();
    bool hitTest(const NavItem& item, sf::Vector2f mp) const;
};
