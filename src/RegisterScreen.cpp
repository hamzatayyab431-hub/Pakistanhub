#include "RegisterScreen.h"

// Local helper to center sf::Text horizontally
static void centerTextHorizontally(sf::Text& text, float centerX) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.0f);
    text.setPosition(centerX, text.getPosition().y);
}

RegisterScreen::RegisterScreen(sf::Font& fnt) : font(fnt) {
    float cardWidth = 440.0f;
    float cardHeight = 540.0f;
    float cardX = (1280.0f - cardWidth) / 2.0f;
    float cardY = (720.0f - cardHeight) / 2.0f;

    // Configure card background
    cardBackground.setPosition(cardX, cardY);
    cardBackground.setSize(sf::Vector2f(cardWidth, cardHeight));
    cardBackground.setFillColor(sf::Color(255, 255, 255, 20)); // ~8% alpha white
    cardBackground.setOutlineColor(sf::Color(255, 255, 255, 80)); // 30% alpha border
    cardBackground.setOutlineThickness(1.0f);

    float centerX = cardX + cardWidth / 2.0f;

    // Title text
    titleText.setFont(font);
    titleText.setCharacterSize(36);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(sf::Color(0, 166, 81)); // Pakistan Green #00A651
    titleText.setString("PakistanHub");
    titleText.setPosition(centerX, cardY + 30.0f);
    centerTextHorizontally(titleText, centerX);

    // Subtitle text
    subtitleText.setFont(font);
    subtitleText.setCharacterSize(16);
    subtitleText.setFillColor(sf::Color(255, 255, 255, 150));
    subtitleText.setString("Create a new account");
    subtitleText.setPosition(centerX, cardY + 80.0f);
    centerTextHorizontally(subtitleText, centerX);

    // Inputs
    float inputWidth = 340.0f;
    float inputHeight = 45.0f;
    float inputX = cardX + (cardWidth - inputWidth) / 2.0f;

    usernameInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 120.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Username"
    );

    displayNameInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 185.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Display Name"
    );

    passwordInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 250.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Password", true
    );

    // Register button
    registerButton = std::make_unique<GlassButton>(
        sf::Vector2f(inputX, cardY + 325.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "REGISTER"
    );

    // Back Link
    backLink.setFont(font);
    backLink.setCharacterSize(14);
    backLink.setFillColor(sf::Color(0, 166, 81, 180));
    backLink.setString("Already have an account? Login");
    backLink.setPosition(centerX, cardY + 395.0f);
    centerTextHorizontally(backLink, centerX);

    // Error message text
    errorText.setFont(font);
    errorText.setCharacterSize(14);
    errorText.setFillColor(sf::Color(220, 53, 69)); // BootStrap Red
    errorText.setString("");
    errorText.setPosition(centerX, cardY + 440.0f);
    centerTextHorizontally(errorText, centerX);
}

void RegisterScreen::draw(sf::RenderWindow& window) {
    window.draw(cardBackground);
    window.draw(titleText);
    window.draw(subtitleText);

    usernameInput->draw(window);
    displayNameInput->draw(window);
    passwordInput->draw(window);
    registerButton->draw(window);

    window.draw(backLink);
    window.draw(errorText);
}

void RegisterScreen::handleEvent(sf::Event& event) {
    usernameInput->handleEvent(event);
    displayNameInput->handleEvent(event);
    passwordInput->handleEvent(event);
    registerButton->handleEvent(event);

    // Link hover highlight effect
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (backLink.getGlobalBounds().contains(mousePos)) {
            backLink.setFillColor(sf::Color(0, 166, 81, 255)); // Highlight green
        } else {
            backLink.setFillColor(sf::Color(0, 166, 81, 180));
        }
    }
}

std::string RegisterScreen::getUsername() const {
    return usernameInput->getText();
}

std::string RegisterScreen::getDisplayName() const {
    return displayNameInput->getText();
}

std::string RegisterScreen::getPassword() const {
    return passwordInput->getText();
}

void RegisterScreen::setErrorMessage(const std::string& error) {
    errorText.setString(error);
    centerTextHorizontally(errorText, cardBackground.getPosition().x + cardBackground.getSize().x / 2.0f);
}

void RegisterScreen::clearFields() {
    usernameInput->clear();
    displayNameInput->clear();
    passwordInput->clear();
    errorText.setString("");
}

bool RegisterScreen::isRegisterClicked(sf::Event& event, sf::RenderWindow& window) {
    return registerButton->isClicked(event, window);
}

bool RegisterScreen::isBackLinkClicked(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return backLink.getGlobalBounds().contains(mousePos);
    }
    return false;
}
