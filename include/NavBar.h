#pragma once
#include <SFML/Graphics.hpp>
#include "GlassButton.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <memory>
#include <string>

// Which screen the nav bar is currently "on" — determines which button is active
enum class NavActive { HOME, SEARCH, PROFILE, NONE };

struct NavBar {
    sf::RectangleShape background;
    sf::Text           logoWhite;   // "Pakistan"
    sf::Text           logoGreen;   // "Hub"
    sf::Text           statusText;
    sf::RectangleShape bottomLine;

    std::unique_ptr<GlassButton> btnHome;
    std::unique_ptr<GlassButton> btnSearch;
    std::unique_ptr<GlassButton> btnProfile;
    std::unique_ptr<GlassButton> btnLogout;

    void init(sf::Font& font, NavActive active, const std::string& username) {
        background.setPosition(0.f, 0.f);
        background.setSize(sf::Vector2f(1280.f, 58.f));

        logoWhite.setFont(font);
        logoWhite.setCharacterSize(22);
        logoWhite.setStyle(sf::Text::Bold);
        logoWhite.setFillColor(Theme::TEXT_WHITE);
        logoWhite.setString("Pakistan");
        logoWhite.setPosition(40.f, 16.f);

        logoGreen.setFont(font);
        logoGreen.setCharacterSize(22);
        logoGreen.setStyle(sf::Text::Bold);
        logoGreen.setFillColor(Theme::GREEN);
        logoGreen.setString("Hub");
        float lw = logoWhite.getGlobalBounds().width;
        logoGreen.setPosition(40.f + lw, 16.f);

        statusText.setFont(font);
        statusText.setCharacterSize(11);
        statusText.setFillColor(Theme::TEXT_MUTED);
        statusText.setString("@" + username);
        statusText.setPosition(40.f, 40.f);

        bottomLine.setPosition(0.f, 58.f);
        bottomLine.setSize(sf::Vector2f(1280.f, 1.f));
        bottomLine.setFillColor(Theme::GREEN);

        auto makeNav = [&](float x, const std::string& lbl, bool isActive) {
            GlassButton::Type t = isActive ? GlassButton::Type::NAV_ACTIVE
                                           : GlassButton::Type::NAV_DEFAULT;
            return std::make_unique<GlassButton>(
                sf::Vector2f(x, 11.f), sf::Vector2f(100.f, 36.f), font, lbl, t);
        };
        btnHome    = makeNav(750.f, "Home",    active == NavActive::HOME);
        btnSearch  = makeNav(860.f, "Search",  active == NavActive::SEARCH);
        btnProfile = makeNav(970.f, "Profile", active == NavActive::PROFILE);
        btnLogout  = std::make_unique<GlassButton>(
            sf::Vector2f(1080.f, 11.f), sf::Vector2f(100.f, 36.f),
            font, "Logout", GlassButton::Type::LOGOUT);
    }

    void setUsername(const std::string& username) {
        statusText.setString("@" + username);
    }

    void draw(sf::RenderWindow& window) {
        // Draw nav background using glass panel
        DrawUtils::drawGlassPanel(window,
            sf::FloatRect(0.f, 0.f, 1280.f, 58.f), 0.f, Theme::GLASS_1);
        window.draw(bottomLine);
        window.draw(logoWhite);
        window.draw(logoGreen);
        window.draw(statusText);
        btnHome->draw(window);
        btnSearch->draw(window);
        btnProfile->draw(window);
        btnLogout->draw(window);
    }

    void handleEvent(sf::Event& event) {
        btnHome->handleEvent(event);
        btnSearch->handleEvent(event);
        btnProfile->handleEvent(event);
        btnLogout->handleEvent(event);
    }

    void update(float dt) {
        btnHome->update(dt);
        btnSearch->update(dt);
        btnProfile->update(dt);
        btnLogout->update(dt);
    }

    bool homeClicked(sf::Event& e)    { return btnHome->isClicked(e); }
    bool searchClicked(sf::Event& e)  { return btnSearch->isClicked(e); }
    bool profileClicked(sf::Event& e) { return btnProfile->isClicked(e); }
    bool logoutClicked(sf::Event& e)  { return btnLogout->isClicked(e); }
};
