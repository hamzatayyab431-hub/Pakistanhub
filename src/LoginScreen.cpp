#include "LoginScreen.h"
#include "GlassPanel.h"
#include "Theme.h"

// Local helper to center sf::Text horizontally
static void centerTextHorizontally(sf::Text& text, float centerX) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.0f);
    text.setPosition(centerX, text.getPosition().y);
}

LoginScreen::LoginScreen(sf::Font& fnt) : font(fnt) {
    float cardWidth = 440.0f;
    float cardHeight = 480.0f;
    float cardX = (1280.0f - cardWidth) / 2.0f;
    float cardY = (720.0f - cardHeight) / 2.0f;

    // Configure card background position
    cardBackground.setPosition(cardX, cardY);
    cardBackground.setSize(sf::Vector2f(cardWidth, cardHeight));

    float centerX = cardX + cardWidth / 2.0f;

    // Title text
    titleText.setFont(font);
    titleText.setCharacterSize(42);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(Theme::GREEN_PRIMARY);
    titleText.setString("PakistanHub");
    titleText.setPosition(centerX, cardY + 30.0f);
    centerTextHorizontally(titleText, centerX);

    // Subtitle text
    subtitleText.setFont(font);
    subtitleText.setCharacterSize(16);
    subtitleText.setFillColor(Theme::TEXT_MUTED);
    subtitleText.setString("Connect with Pakistan");
    subtitleText.setPosition(centerX, cardY + 85.0f);
    centerTextHorizontally(subtitleText, centerX);

    // Divider line
    sf::RectangleShape divider(sf::Vector2f(cardWidth, 1.0f));
    divider.setPosition(cardX, cardY + 120.0f);
    divider.setFillColor(Theme::GLASS_BORDER);

    // Inputs
    float inputWidth = 360.0f;
    float inputHeight = 45.0f;
    float inputX = cardX + (cardWidth - inputWidth) / 2.0f;

    usernameInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 150.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Username"
    );

    passwordInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 220.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Password", true
    );

    loginButton = std::make_unique<GlassButton>(
        sf::Vector2f(inputX, cardY + 300.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "LOGIN"
    );

    // Register Link
    registerLink.setFont(font);
    registerLink.setCharacterSize(14);
    registerLink.setFillColor(Theme::TEXT_MUTED);
    registerLink.setString("Don't have an account? Register");
    registerLink.setPosition(centerX, cardY + 370.0f);
    centerTextHorizontally(registerLink, centerX);

    // Error text
    errorText.setFont(font);
    errorText.setCharacterSize(14);
    errorText.setFillColor(Theme::ACCENT_RED);
    errorText.setString("");
    errorText.setPosition(centerX, cardY + 410.0f);
}

void LoginScreen::draw(sf::RenderWindow& window) {
    // Use GlassPanel to draw the card
    GlassPanel::draw(window, cardBackground.getGlobalBounds(), false, 1.0f);

    window.draw(titleText);
    window.draw(subtitleText);

    // Draw a divider line
    sf::RectangleShape divider(sf::Vector2f(cardBackground.getSize().x, 1.0f));
    divider.setPosition(cardBackground.getPosition().x, cardBackground.getPosition().y + 120.0f);
    divider.setFillColor(Theme::GLASS_BORDER);
    window.draw(divider);

    usernameInput->draw(window);
    passwordInput->draw(window);
    loginButton->draw(window);

    window.draw(registerLink);
    window.draw(errorText);
}

void LoginScreen::handleEvent(sf::Event& event) {
    usernameInput->handleEvent(event);
    passwordInput->handleEvent(event);
    loginButton->handleEvent(event);

    // Link hover highlight effect
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (registerLink.getGlobalBounds().contains(mousePos)) {
            registerLink.setFillColor(sf::Color(0, 166, 81, 255)); // Highlight green
        } else {
            registerLink.setFillColor(sf::Color(0, 166, 81, 180));
        }
    }
}

std::string LoginScreen::getUsername() const {
    return usernameInput->getText();
}

std::string LoginScreen::getPassword() const {
    return passwordInput->getText();
}

void LoginScreen::setErrorMessage(const std::string& error) {
    errorText.setString(error);
    centerTextHorizontally(errorText, cardBackground.getPosition().x + cardBackground.getSize().x / 2.0f);
}

void LoginScreen::clearFields() {
    usernameInput->clear();
    passwordInput->clear();
    errorText.setString("");
}

bool LoginScreen::isLoginClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window; // Suppress unused warning
    return loginButton->isClicked(event);
}

bool LoginScreen::isRegisterLinkClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return registerLink.getGlobalBounds().contains(mousePos);
    }
    return false;
}
