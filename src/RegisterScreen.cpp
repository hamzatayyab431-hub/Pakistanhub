#include "RegisterScreen.h"
#include "GlassPanel.h"
#include "Theme.h"

// Local helper to center sf::Text horizontally
static void centerTextHorizontally(sf::Text& text, float centerX) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.0f);
    text.setPosition(centerX, text.getPosition().y);
}

RegisterScreen::RegisterScreen(sf::Font& fnt) : font(fnt) {
    float cardWidth = 440.0f;
    float cardHeight = 660.0f;
    float cardX = (1280.0f - cardWidth) / 2.0f;
    float cardY = (720.0f - cardHeight) / 2.0f;

    // Configure card background
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
    subtitleText.setString("Create a new account");
    subtitleText.setPosition(centerX, cardY + 80.0f);
    centerTextHorizontally(subtitleText, centerX);

    // Divider line
    sf::RectangleShape divider(sf::Vector2f(cardWidth, 1.0f));
    divider.setPosition(cardX, cardY + 110.0f);
    divider.setFillColor(Theme::GLASS_BORDER);

    // Inputs
    float inputWidth = 360.0f;
    float inputHeight = 45.0f;
    float inputX = cardX + (cardWidth - inputWidth) / 2.0f;

    usernameInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 130.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Username"
    );

    displayNameInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 190.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Display Name"
    );

    bioInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 250.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Bio"
    );

    cityInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 310.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "City"
    );

    passwordInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 370.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Password", true
    );

    // Strength indicator
    strengthBarBg.setPosition(inputX, cardY + 425.0f);
    strengthBarBg.setSize(sf::Vector2f(inputWidth, 4.0f));
    strengthBarBg.setFillColor(sf::Color(255, 255, 255, 20));

    strengthBarFill.setPosition(inputX, cardY + 425.0f);
    strengthBarFill.setSize(sf::Vector2f(0.0f, 4.0f));

    confirmPasswordInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, cardY + 445.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "Confirm Password", true
    );

    // Register button
    registerButton = std::make_unique<GlassButton>(
        sf::Vector2f(inputX, cardY + 515.0f),
        sf::Vector2f(inputWidth, inputHeight),
        font, "REGISTER"
    );

    // Back Link
    backLink.setFont(font);
    backLink.setCharacterSize(14);
    backLink.setFillColor(Theme::TEXT_MUTED);
    backLink.setString("Already have an account? Login");
    backLink.setPosition(centerX, cardY + 580.0f);
    centerTextHorizontally(backLink, centerX);

    // Error message text
    errorText.setFont(font);
    errorText.setCharacterSize(14);
    errorText.setFillColor(Theme::ACCENT_RED);
    errorText.setString("");
    errorText.setPosition(centerX, cardY + 620.0f);
}

void RegisterScreen::updatePasswordStrength() {
    std::string pwd = passwordInput->getText();
    float width = strengthBarBg.getSize().x;
    if (pwd.empty()) {
        strengthBarFill.setSize(sf::Vector2f(0.0f, 4.0f));
    } else if (pwd.length() < 6) {
        strengthBarFill.setSize(sf::Vector2f(width * 0.33f, 4.0f));
        strengthBarFill.setFillColor(Theme::ACCENT_RED);
    } else if (pwd.length() < 10) {
        strengthBarFill.setSize(sf::Vector2f(width * 0.66f, 4.0f));
        strengthBarFill.setFillColor(sf::Color(255, 200, 50));
    } else {
        strengthBarFill.setSize(sf::Vector2f(width, 4.0f));
        strengthBarFill.setFillColor(Theme::GREEN_PRIMARY);
    }
}

void RegisterScreen::update() {
    updatePasswordStrength();
}

void RegisterScreen::draw(sf::RenderWindow& window) {
    GlassPanel::draw(window, cardBackground.getGlobalBounds(), false, 1.0f);

    window.draw(titleText);
    window.draw(subtitleText);

    sf::RectangleShape divider(sf::Vector2f(cardBackground.getSize().x, 1.0f));
    divider.setPosition(cardBackground.getPosition().x, cardBackground.getPosition().y + 110.0f);
    divider.setFillColor(Theme::GLASS_BORDER);
    window.draw(divider);

    usernameInput->draw(window);
    displayNameInput->draw(window);
    bioInput->draw(window);
    cityInput->draw(window);
    passwordInput->draw(window);
    
    window.draw(strengthBarBg);
    window.draw(strengthBarFill);

    confirmPasswordInput->draw(window);
    registerButton->draw(window);

    window.draw(backLink);
    window.draw(errorText);
}

void RegisterScreen::handleEvent(sf::Event& event) {
    usernameInput->handleEvent(event);
    displayNameInput->handleEvent(event);
    bioInput->handleEvent(event);
    cityInput->handleEvent(event);
    passwordInput->handleEvent(event);
    confirmPasswordInput->handleEvent(event);
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

std::string RegisterScreen::getBio() const {
    return bioInput->getText();
}

std::string RegisterScreen::getCity() const {
    return cityInput->getText();
}

std::string RegisterScreen::getPassword() const {
    return passwordInput->getText();
}

std::string RegisterScreen::getConfirmPassword() const {
    return confirmPasswordInput->getText();
}

void RegisterScreen::setErrorMessage(const std::string& error) {
    errorText.setString(error);
    centerTextHorizontally(errorText, cardBackground.getPosition().x + cardBackground.getSize().x / 2.0f);
}

void RegisterScreen::clearFields() {
    usernameInput->clear();
    displayNameInput->clear();
    bioInput->clear();
    cityInput->clear();
    passwordInput->clear();
    confirmPasswordInput->clear();
    errorText.setString("");
    updatePasswordStrength();
}

bool RegisterScreen::isRegisterClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window; // Suppress unused warning
    return registerButton->isClicked(event);
}

bool RegisterScreen::isBackLinkClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return backLink.getGlobalBounds().contains(mousePos);
    }
    return false;
}
