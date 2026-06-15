#include "LoginScreen.h"
#include "DrawUtils.h"
#include "Theme.h"

static void centerText(sf::Text& t, float cx) {
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.left + b.width / 2.f, 0.f);
    t.setPosition(cx, t.getPosition().y);
}

LoginScreen::LoginScreen(sf::Font& fnt) : font(fnt), errorAlpha(0.f) {
    const float cw = 440.f, ch = 420.f;
    const float cx = (1280.f - cw) / 2.f;
    const float cy = (720.f  - ch) / 2.f;
    const float centerX = cx + cw / 2.f;

    cardBackground.setPosition(cx, cy);
    cardBackground.setSize(sf::Vector2f(cw, ch));

    // Title
    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(Theme::GREEN);
    titleText.setString("PakistanHub");
    titleText.setPosition(centerX, cy + 28.f);
    centerText(titleText, centerX);

    // Subtitle
    subtitleText.setFont(font);
    subtitleText.setCharacterSize(14);
    subtitleText.setFillColor(Theme::TEXT_MUTED);
    subtitleText.setString("Connect with Pakistan");
    subtitleText.setPosition(centerX, cy + 76.f);
    centerText(subtitleText, centerX);

    const float iw = 360.f, ih = 46.f;
    const float ix = cx + (cw - iw) / 2.f;

    usernameInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, cy + 140.f), sf::Vector2f(iw, ih), font, "Username");

    passwordInput = std::make_unique<TextInput>(
        sf::Vector2f(ix, cy + 200.f), sf::Vector2f(iw, ih), font, "Password", true);

    loginButton = std::make_unique<GlassButton>(
        sf::Vector2f(ix, cy + 268.f), sf::Vector2f(iw, ih), font, "LOGIN");

    // Register link
    registerLink.setFont(font);
    registerLink.setCharacterSize(14);
    registerLink.setFillColor(Theme::TEXT_MUTED);
    registerLink.setString("Don't have an account? ");
    registerLink.setPosition(centerX - 90.f, cy + 338.f);

    registerLinkHighlight.setFont(font);
    registerLinkHighlight.setCharacterSize(14);
    registerLinkHighlight.setFillColor(Theme::GREEN);
    registerLinkHighlight.setStyle(sf::Text::Bold);
    registerLinkHighlight.setString("Register");

    // Error text
    errorText.setFont(font);
    errorText.setCharacterSize(13);
    errorText.setFillColor(Theme::ERROR);
    errorText.setString("");
    errorText.setPosition(centerX, cy + 374.f);

    // Underline for hover effect on "Register"
    registerUnderline.setSize(sf::Vector2f(0.f, 1.f));
    registerUnderline.setFillColor(Theme::GREEN);

    registerHovered = false;

    // Animate error alpha
    errorAlphaLerp.speed = 8.f;
    errorAlphaLerp.snap(0.f);
}

void LoginScreen::draw(sf::RenderWindow& window) {
    auto bounds = cardBackground.getGlobalBounds();

    // Card shadow
    sf::RectangleShape shadow(sf::Vector2f(bounds.width, bounds.height));
    shadow.setPosition(bounds.left + 4.f, bounds.top + 4.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 60));
    window.draw(shadow);

    // Glass card
    DrawUtils::drawGlassPanel(window, bounds, 12.f, Theme::GLASS_2);

    window.draw(titleText);
    window.draw(subtitleText);

    // Divider
    sf::RectangleShape div(sf::Vector2f(bounds.width, 1.f));
    div.setPosition(bounds.left, bounds.top + 118.f);
    div.setFillColor(Theme::GLASS_BORDER);
    window.draw(div);

    usernameInput->draw(window);
    passwordInput->draw(window);
    loginButton->draw(window);

    // Register link
    window.draw(registerLink);

    // Position highlight next to plain text
    sf::FloatRect rlb = registerLink.getGlobalBounds();
    registerLinkHighlight.setPosition(rlb.left + rlb.width, registerLink.getPosition().y);
    window.draw(registerLinkHighlight);

    if (registerHovered) {
        sf::FloatRect hlb = registerLinkHighlight.getGlobalBounds();
        registerUnderline.setSize(sf::Vector2f(hlb.width, 1.f));
        registerUnderline.setPosition(hlb.left, hlb.top + hlb.height + 1.f);
        window.draw(registerUnderline);
    }

    // Error text with animated alpha
    if (!errorText.getString().isEmpty()) {
        sf::Color ec = Theme::ERROR;
        ec.a = static_cast<sf::Uint8>(errorAlphaLerp.current);
        errorText.setFillColor(ec);
        window.draw(errorText);
    }
}

void LoginScreen::handleEvent(sf::Event& event) {
    usernameInput->handleEvent(event);
    passwordInput->handleEvent(event);
    loginButton->handleEvent(event);

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
    errorAlphaLerp.update(dt);
}

// Legacy no-arg update (called by old UIComponent::update())
void LoginScreen::update() { update(0.016f); }

std::string LoginScreen::getUsername() const { return usernameInput->getText(); }
std::string LoginScreen::getPassword() const { return passwordInput->getText(); }

void LoginScreen::setErrorMessage(const std::string& msg) {
    float cx = cardBackground.getPosition().x + cardBackground.getSize().x / 2.f;
    errorText.setString(msg);
    sf::FloatRect b = errorText.getLocalBounds();
    errorText.setOrigin(b.left + b.width / 2.f, 0.f);
    errorText.setPosition(cx, errorText.getPosition().y);
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
               registerLinkHighlight.getGlobalBounds().contains(mp);
    }
    return false;
}
