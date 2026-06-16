#include "LoginScreen.h"
#include "DrawUtils.h"
#include "Theme.h"

LoginScreen::LoginScreen(sf::Font& fnt) : font(fnt), errorAlpha(0.f) {
    // ── Right card (authentication) ──────────────────────────────────────────
    const float cw = 420.f, ch = 440.f;
    const float cx = 1280.f - cw - 60.f;
    const float cy = (720.f - ch) / 2.f;
    const float centerX = cx + cw / 2.f;

    cardBg.setPosition(cx, cy);
    cardBg.setSize(sf::Vector2f(cw, ch));

    // "Welcome Back"
    titleText.setFont(font);
    titleText.setCharacterSize(26);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(Theme::TEXT_WHITE);
    titleText.setString("Welcome Back");
    titleText.setPosition(centerX, cy + 28.f);
    DrawUtils::centreText(titleText, centerX);

    subtitleText.setFont(font);
    subtitleText.setCharacterSize(13);
    subtitleText.setFillColor(Theme::TEXT_MUTED);
    subtitleText.setString("Sign in to continue");
    subtitleText.setPosition(centerX, cy + 62.f);
    DrawUtils::centreText(subtitleText, centerX);

    const float iw = 340.f, ih = 46.f;
    const float ix = cx + (cw - iw) / 2.f;

    usernameInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, cy + 106.f), sf::Vector2f(iw, ih), font, "Username");
    passwordInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, cy + 164.f), sf::Vector2f(iw, ih), font, "Password", true);

    loginButton = std::make_unique<GlassButton>(
        sf::Vector2f(ix, cy + 232.f), sf::Vector2f(iw, 46.f), font, "Sign In");

    // Divider "or"
    orText.setFont(font);
    orText.setCharacterSize(12);
    orText.setFillColor(Theme::TEXT_DIM);
    orText.setString("or");
    orText.setPosition(centerX, cy + 290.f);
    DrawUtils::centreText(orText, centerX);

    createAccountBtn = std::make_unique<GlassButton>(
        sf::Vector2f(ix, cy + 312.f), sf::Vector2f(iw, 44.f),
        font, "Create Account", GlassButton::Type::NAV_DEFAULT);

    // "Don't have an account?" link
    registerLink.setFont(font);
    registerLink.setCharacterSize(13);
    registerLink.setFillColor(Theme::TEXT_MUTED);
    registerLink.setString("Don't have an account?  ");
    registerLink.setPosition(cx + 40.f, cy + 370.f);

    registerLinkHighlight.setFont(font);
    registerLinkHighlight.setCharacterSize(13);
    registerLinkHighlight.setStyle(sf::Text::Bold);
    registerLinkHighlight.setFillColor(Theme::GREEN);
    registerLinkHighlight.setString("Create one");
    registerHovered = false;

    registerUnderline.setSize(sf::Vector2f(0.f, 1.f));
    registerUnderline.setFillColor(Theme::GREEN);

    // Error text
    errorText.setFont(font);
    errorText.setCharacterSize(12);
    errorText.setFillColor(Theme::ERROR);
    errorText.setString("");
    errorText.setPosition(centerX, cy + 396.f);

    errorAlphaLerp.speed = 8.f;
    errorAlphaLerp.snap(0.f);

    // ── Left side (branding) ─────────────────────────────────────────────────
    leftBg.setSize(sf::Vector2f(cx - 0.f, 720.f));
    leftBg.setPosition(0.f, 0.f);
    leftBg.setFillColor(sf::Color::Transparent);

    brandLogo.setFont(font);
    brandLogo.setCharacterSize(36);
    brandLogo.setStyle(sf::Text::Bold);
    brandLogo.setFillColor(Theme::GREEN);
    brandLogo.setString("PakistanHub");
    brandLogo.setPosition(60.f, 180.f);

    brandTagline.setFont(font);
    brandTagline.setCharacterSize(15);
    brandTagline.setFillColor(Theme::TEXT_MUTED);
    brandTagline.setString("Connect with Pakistan.");
    brandTagline.setPosition(60.f, 228.f);

    brandDesc.setFont(font);
    brandDesc.setCharacterSize(13);
    brandDesc.setFillColor(Theme::TEXT_DIM);
    brandDesc.setString("A platform to share, connect, and\nbuild communities across Pakistan.");
    brandDesc.setPosition(60.f, 260.f);

    // Feature bullets
    const char* bulletTexts[3] = {
        "Share Updates",
        "Build Communities",
        "Discover People"
    };
    const char* bulletDescs[3] = {
        "Post thoughts, photos and videos.",
        "Join groups and connect with\nlike-minded people.",
        "Find interesting people and\nexpand your network."
    };
    for (int i = 0; i < 3; ++i) {
        bulletTitle[i].setFont(font);
        bulletTitle[i].setCharacterSize(13);
        bulletTitle[i].setStyle(sf::Text::Bold);
        bulletTitle[i].setFillColor(Theme::TEXT_WHITE);
        bulletTitle[i].setString(bulletTexts[i]);
        bulletTitle[i].setPosition(92.f, 320.f + i * 72.f);

        bulletDesc[i].setFont(font);
        bulletDesc[i].setCharacterSize(12);
        bulletDesc[i].setFillColor(Theme::TEXT_DIM);
        bulletDesc[i].setString(bulletDescs[i]);
        bulletDesc[i].setPosition(92.f, 338.f + i * 72.f);

        bulletDot[i].setRadius(6.f);
        bulletDot[i].setOrigin(6.f, 6.f);
        bulletDot[i].setFillColor(Theme::GREEN);
        bulletDot[i].setPosition(72.f, 328.f + i * 72.f);
    }

    copyrightText.setFont(font);
    copyrightText.setCharacterSize(11);
    copyrightText.setFillColor(Theme::TEXT_DIM);
    copyrightText.setString("© 2024 PakistanHub. All rights reserved.");
    copyrightText.setPosition(60.f, 690.f);
}

void LoginScreen::draw(sf::RenderWindow& window) {
    // ── Left panel background ─────────────────────────────────────────────────
    sf::RectangleShape leftPanel(sf::Vector2f(cardBg.getPosition().x, 720.f));
    leftPanel.setPosition(0.f, 0.f);
    leftPanel.setFillColor(Theme::BG_SECONDARY);
    window.draw(leftPanel);

    // Vertical divider
    sf::RectangleShape vdiv(sf::Vector2f(1.f, 720.f));
    vdiv.setPosition(cardBg.getPosition().x, 0.f);
    vdiv.setFillColor(Theme::DIVIDER);
    window.draw(vdiv);

    window.draw(brandLogo);
    window.draw(brandTagline);
    window.draw(brandDesc);
    for (int i = 0; i < 3; ++i) {
        window.draw(bulletDot[i]);
        window.draw(bulletTitle[i]);
        window.draw(bulletDesc[i]);
    }
    window.draw(copyrightText);

    // ── Right auth card ───────────────────────────────────────────────────────
    auto bounds = cardBg.getGlobalBounds();
    // Shadow
    sf::RectangleShape shadow(sf::Vector2f(bounds.width, bounds.height));
    shadow.setPosition(bounds.left + 3.f, bounds.top + 3.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 60));
    window.draw(shadow);

    DrawUtils::drawCard(window, bounds, 20.f, Theme::BG_SURFACE,
                        Theme::DIVIDER, 1.f);

    window.draw(titleText);
    window.draw(subtitleText);

    DrawUtils::drawDivider(window, bounds.left + 24.f, bounds.top + 96.f, bounds.width - 48.f);

    usernameInput->draw(window);
    passwordInput->draw(window);
    loginButton->draw(window);

    // "or" divider
    DrawUtils::drawDivider(window, bounds.left + 24.f, bounds.top + 286.f, bounds.width / 2.f - 36.f);
    DrawUtils::drawDivider(window, bounds.left + bounds.width / 2.f + 16.f, bounds.top + 286.f, bounds.width / 2.f - 36.f);
    window.draw(orText);

    createAccountBtn->draw(window);

    window.draw(registerLink);
    sf::FloatRect rlb = registerLink.getGlobalBounds();
    registerLinkHighlight.setPosition(rlb.left + rlb.width, registerLink.getPosition().y);
    window.draw(registerLinkHighlight);

    if (registerHovered) {
        sf::FloatRect hlb = registerLinkHighlight.getGlobalBounds();
        registerUnderline.setSize(sf::Vector2f(hlb.width, 1.f));
        registerUnderline.setPosition(hlb.left, hlb.top + hlb.height + 1.f);
        window.draw(registerUnderline);
    }

    if (!errorText.getString().isEmpty()) {
        sf::Color ec = Theme::ERROR;
        ec.a = static_cast<sf::Uint8>(errorAlphaLerp.current);
        errorText.setFillColor(ec);
        float cx = cardBg.getPosition().x + cardBg.getSize().x / 2.f;
        DrawUtils::centreText(errorText, cx);
        window.draw(errorText);
    }
}

void LoginScreen::handleEvent(sf::Event& event) {
    usernameInput->handleEvent(event);
    passwordInput->handleEvent(event);
    loginButton->handleEvent(event);
    createAccountBtn->handleEvent(event);

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        registerHovered = registerLinkHighlight.getGlobalBounds().contains(mp);
    }
}

void LoginScreen::update(float dt) {
    usernameInput->update(dt);
    passwordInput->update(dt);
    loginButton->update(dt);
    createAccountBtn->update(dt);
    errorAlphaLerp.update(dt);
}

void LoginScreen::update() { update(0.016f); }

std::string LoginScreen::getUsername() const { return usernameInput->getText(); }
std::string LoginScreen::getPassword() const { return passwordInput->getText(); }

void LoginScreen::setErrorMessage(const std::string& msg) {
    float cx = cardBg.getPosition().x + cardBg.getSize().x / 2.f;
    errorText.setString(msg);
    DrawUtils::centreText(errorText, cx);
    errorAlphaLerp.snap(0.f);
    errorAlphaLerp.setTarget(255.f);
}

void LoginScreen::clearFields() {
    usernameInput->clear();
    passwordInput->clear();
    errorText.setString("");
    errorAlphaLerp.snap(0.f);
}

bool LoginScreen::isLoginClicked(sf::Event& event, sf::RenderWindow&) {
    return loginButton->isClicked(event);
}

bool LoginScreen::isRegisterLinkClicked(sf::Event& event, sf::RenderWindow&) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        return registerLink.getGlobalBounds().contains(mp) ||
               registerLinkHighlight.getGlobalBounds().contains(mp) ||
               createAccountBtn->isClicked(event);
    }
    return false;
}
