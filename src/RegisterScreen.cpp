#include "RegisterScreen.h"
#include "DrawUtils.h"
#include "Theme.h"

static void centerText(sf::Text& t, float cx) {
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, 0.f);
    t.setPosition(cx, t.getPosition().y);
}

RegisterScreen::RegisterScreen(sf::Font& fnt) : font(fnt) {
    const float cw = 440.f, ch = 620.f;
    const float cx = (1280.f - cw) / 2.f;
    const float cy = (720.f  - ch) / 2.f;
    const float centerX = cx + cw / 2.f;

    cardBackground.setPosition(cx, cy);
    cardBackground.setSize(sf::Vector2f(cw, ch));

    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(Theme::GREEN);
    titleText.setString("PakistanHub");
    titleText.setPosition(centerX, cy + 24.f);
    centerText(titleText, centerX);

    subtitleText.setFont(font);
    subtitleText.setCharacterSize(14);
    subtitleText.setFillColor(Theme::TEXT_MUTED);
    subtitleText.setString("Create a new account");
    subtitleText.setPosition(centerX, cy + 70.f);
    centerText(subtitleText, centerX);

    const float iw = 360.f, ih = 44.f;
    const float ix = cx + (cw - iw) / 2.f;
    float y = cy + 108.f;
    const float gap = 50.f;

    usernameInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "Username");
    y += gap;

    displayNameInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "Display Name");
    y += gap;

    bioInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "Bio");
    y += gap;

    cityInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "City");
    y += gap;

    passwordInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "Password", true);
    y += ih + 6.f;

    // Password strength: 3 segments
    const float segW = 108.f, segH = 4.f, segGap = 6.f;
    for (int i = 0; i < 3; ++i) {
        strengthSeg[i].setSize(sf::Vector2f(segW, segH));
        strengthSeg[i].setPosition(ix + i * (segW + segGap), y);
        strengthSeg[i].setFillColor(Theme::TEXT_DIM);
    }
    strengthSegAlpha[0] = strengthSegAlpha[1] = strengthSegAlpha[2] = 0.f;
    y += 12.f;

    confirmPasswordInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "Confirm Password", true);
    y += gap + 4.f;

    registerButton = std::make_unique<GlassButton>(
        sf::Vector2f(ix, y), sf::Vector2f(iw, ih), font, "REGISTER");
    y += ih + 10.f;

    backLink.setFont(font);
    backLink.setCharacterSize(14);
    backLink.setFillColor(Theme::TEXT_MUTED);
    backLink.setString("Already have an account? ");
    backLink.setPosition(cx + 50.f, y);

    backLinkHighlight.setFont(font);
    backLinkHighlight.setCharacterSize(14);
    backLinkHighlight.setFillColor(Theme::GREEN);
    backLinkHighlight.setStyle(sf::Text::Bold);
    backLinkHighlight.setString("Login");
    backHovered = false;

    backUnderline.setSize(sf::Vector2f(0.f, 1.f));
    backUnderline.setFillColor(Theme::GREEN);

    errorText.setFont(font);
    errorText.setCharacterSize(13);
    errorText.setFillColor(Theme::ERROR);
    errorText.setString("");
    errorText.setPosition(centerX, y + 24.f);

    errorAlphaLerp.speed = 8.f;
    errorAlphaLerp.snap(0.f);
}

void RegisterScreen::updatePasswordStrength() {
    std::string pwd = passwordInput->getText();

    // Determine how many segments are active
    int active = 0;
    if (!pwd.empty()) {
        active = 1;
        if (pwd.length() >= 6)  active = 2;
        if (pwd.length() >= 10) active = 3;
    }

    sf::Color colors[3] = { Theme::ERROR, Theme::WARNING, Theme::GREEN };
    for (int i = 0; i < 3; ++i) {
        sf::Color c = (i < active) ? colors[active - 1] : Theme::TEXT_DIM;
        // Animate segment alpha in
        float targetA = pwd.empty() ? 0.f : 255.f;
        strengthSegAlpha[i] += (targetA - strengthSegAlpha[i]) * 0.1f;
        c.a = static_cast<sf::Uint8>(strengthSegAlpha[i]);
        strengthSeg[i].setFillColor(c);
    }
}

void RegisterScreen::update(float dt) {
    usernameInput->update(dt);
    displayNameInput->update(dt);
    bioInput->update(dt);
    cityInput->update(dt);
    passwordInput->update(dt);
    confirmPasswordInput->update(dt);
    registerButton->update(dt);
    errorAlphaLerp.update(dt);
    updatePasswordStrength();
}

void RegisterScreen::update() { update(0.016f); }

void RegisterScreen::draw(sf::RenderWindow& window) {
    auto bounds = cardBackground.getGlobalBounds();

    // Shadow
    sf::RectangleShape shadow(sf::Vector2f(bounds.width, bounds.height));
    shadow.setPosition(bounds.left + 4.f, bounds.top + 4.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 60));
    window.draw(shadow);

    DrawUtils::drawGlassPanel(window, bounds, 12.f, Theme::GLASS_2);

    window.draw(titleText);
    window.draw(subtitleText);

    sf::RectangleShape div(sf::Vector2f(bounds.width, 1.f));
    div.setPosition(bounds.left, bounds.top + 102.f);
    div.setFillColor(Theme::GLASS_BORDER);
    window.draw(div);

    usernameInput->draw(window);
    displayNameInput->draw(window);
    bioInput->draw(window);
    cityInput->draw(window);
    passwordInput->draw(window);

    for (auto& seg : strengthSeg) window.draw(seg);

    confirmPasswordInput->draw(window);
    registerButton->draw(window);

    window.draw(backLink);
    sf::FloatRect blb = backLink.getGlobalBounds();
    backLinkHighlight.setPosition(blb.left + blb.width, backLink.getPosition().y);
    window.draw(backLinkHighlight);

    if (backHovered) {
        sf::FloatRect hlb = backLinkHighlight.getGlobalBounds();
        backUnderline.setSize(sf::Vector2f(hlb.width, 1.f));
        backUnderline.setPosition(hlb.left, hlb.top + hlb.height + 1.f);
        window.draw(backUnderline);
    }

    if (!errorText.getString().isEmpty()) {
        sf::Color ec = Theme::ERROR;
        ec.a = static_cast<sf::Uint8>(errorAlphaLerp.current);
        errorText.setFillColor(ec);
        window.draw(errorText);
    }
}

void RegisterScreen::handleEvent(sf::Event& event) {
    usernameInput->handleEvent(event);
    displayNameInput->handleEvent(event);
    bioInput->handleEvent(event);
    cityInput->handleEvent(event);
    passwordInput->handleEvent(event);
    confirmPasswordInput->handleEvent(event);
    registerButton->handleEvent(event);

    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        backHovered = backLinkHighlight.getGlobalBounds().contains(mp);
    }
}

std::string RegisterScreen::getUsername()        const { return usernameInput->getText(); }
std::string RegisterScreen::getDisplayName()     const { return displayNameInput->getText(); }
std::string RegisterScreen::getBio()             const { return bioInput->getText(); }
std::string RegisterScreen::getCity()            const { return cityInput->getText(); }
std::string RegisterScreen::getPassword()        const { return passwordInput->getText(); }
std::string RegisterScreen::getConfirmPassword() const { return confirmPasswordInput->getText(); }

void RegisterScreen::setErrorMessage(const std::string& msg) {
    float cx = cardBackground.getPosition().x + cardBackground.getSize().x / 2.f;
    errorText.setString(msg);
    sf::FloatRect b = errorText.getLocalBounds();
    errorText.setOrigin(b.left + b.width / 2.f, 0.f);
    errorText.setPosition(cx, errorText.getPosition().y);
    errorAlphaLerp.snap(0.f);
    errorAlphaLerp.setTarget(255.f);
}

void RegisterScreen::clearFields() {
    usernameInput->clear();
    displayNameInput->clear();
    bioInput->clear();
    cityInput->clear();
    passwordInput->clear();
    confirmPasswordInput->clear();
    errorText.setString("");
    errorAlphaLerp.snap(0.f);
}

bool RegisterScreen::isRegisterClicked(sf::Event& event, sf::RenderWindow&) {
    return registerButton->isClicked(event);
}

bool RegisterScreen::isBackLinkClicked(sf::Event& event, sf::RenderWindow&) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        return backLink.getGlobalBounds().contains(mp) ||
               backLinkHighlight.getGlobalBounds().contains(mp);
    }
    return false;
}
