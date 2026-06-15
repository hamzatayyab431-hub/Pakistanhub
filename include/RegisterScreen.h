#ifndef REGISTERSCREEN_H
#define REGISTERSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include <memory>

class RegisterScreen : public UIComponent {
private:
    sf::Font& font;

    // Card background shape
    sf::RectangleShape cardBackground;

    // UI elements
    sf::Text titleText;
    sf::Text subtitleText;
    std::unique_ptr<TextInput> usernameInput;
    std::unique_ptr<TextInput> displayNameInput;
    std::unique_ptr<TextInput> passwordInput;
    std::unique_ptr<GlassButton> registerButton;
    sf::Text backLink;
    sf::Text errorText;

public:
    RegisterScreen(sf::Font& fnt);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    std::string getUsername() const;
    std::string getDisplayName() const;
    std::string getPassword() const;
    void setErrorMessage(const std::string& error);
    void clearFields();

    bool isRegisterClicked(sf::Event& event, sf::RenderWindow& window);
    bool isBackLinkClicked(sf::Event& event, sf::RenderWindow& window);
};

#endif // REGISTERSCREEN_H
