#ifndef LOGINSCREEN_H
#define LOGINSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include <memory>

class LoginScreen : public UIComponent {
private:
    sf::Font& font;

    // Card background shape
    sf::RectangleShape cardBackground;

    // UI elements
    sf::Text titleText;
    sf::Text subtitleText;
    std::unique_ptr<TextInput> usernameInput;
    std::unique_ptr<TextInput> passwordInput;
    std::unique_ptr<GlassButton> loginButton;
    sf::Text registerLink;
    sf::Text errorText;

public:
    LoginScreen(sf::Font& fnt);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    std::string getUsername() const;
    std::string getPassword() const;
    void setErrorMessage(const std::string& error);
    void clearFields();

    bool isLoginClicked(sf::Event& event, sf::RenderWindow& window);
    bool isRegisterLinkClicked(sf::Event& event, sf::RenderWindow& window);
};

#endif // LOGINSCREEN_H
