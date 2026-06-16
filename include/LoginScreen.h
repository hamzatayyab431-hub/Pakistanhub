#ifndef LOGINSCREEN_H
#define LOGINSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "Animator.h"
#include <memory>

class LoginScreen : public UIComponent {
private:
    sf::Font& font;

    // Right card
    sf::RectangleShape cardBg;
    sf::Text           titleText;
    sf::Text           subtitleText;
    std::unique_ptr<TextInput>   usernameInput;
    std::unique_ptr<TextInput>   passwordInput;
    std::unique_ptr<GlassButton> loginButton;
    std::unique_ptr<GlassButton> createAccountBtn;
    sf::Text           orText;
    sf::Text           registerLink;
    sf::Text           registerLinkHighlight;
    sf::RectangleShape registerUnderline;
    bool               registerHovered = false;
    sf::Text           errorText;
    float              errorAlpha;
    Lerp               errorAlphaLerp;

    // Left branding panel
    sf::RectangleShape leftBg;
    sf::Text           brandLogo;
    sf::Text           brandTagline;
    sf::Text           brandDesc;
    sf::Text           bulletTitle[3];
    sf::Text           bulletDesc[3];
    sf::CircleShape    bulletDot[3];
    sf::Text           copyrightText;

public:
    LoginScreen(sf::Font& fnt);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;
    void update(float dt);

    std::string getUsername() const;
    std::string getPassword() const;
    void setErrorMessage(const std::string& error);
    void clearFields();

    bool isLoginClicked(sf::Event& event, sf::RenderWindow& window);
    bool isRegisterLinkClicked(sf::Event& event, sf::RenderWindow& window);
};

#endif // LOGINSCREEN_H
