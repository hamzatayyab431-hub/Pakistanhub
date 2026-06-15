#ifndef REGISTERSCREEN_H
#define REGISTERSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "Animator.h"
#include <memory>

class RegisterScreen : public UIComponent {
private:
    sf::Font& font;

    sf::RectangleShape cardBackground;
    sf::Text           titleText;
    sf::Text           subtitleText;

    std::unique_ptr<TextInput>   usernameInput;
    std::unique_ptr<TextInput>   displayNameInput;
    std::unique_ptr<TextInput>   bioInput;
    std::unique_ptr<TextInput>   cityInput;
    std::unique_ptr<TextInput>   passwordInput;
    std::unique_ptr<TextInput>   confirmPasswordInput;
    std::unique_ptr<GlassButton> registerButton;

    // Password strength — 3 segments
    sf::RectangleShape strengthSeg[3];
    float              strengthSegAlpha[3];

    sf::Text           backLink;
    sf::Text           backLinkHighlight;
    sf::RectangleShape backUnderline;
    bool               backHovered;

    sf::Text           errorText;
    Lerp               errorAlphaLerp;

    void updatePasswordStrength();

public:
    RegisterScreen(sf::Font& fnt);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;
    void update(float dt);

    std::string getUsername()        const;
    std::string getDisplayName()     const;
    std::string getBio()             const;
    std::string getCity()            const;
    std::string getPassword()        const;
    std::string getConfirmPassword() const;
    void setErrorMessage(const std::string& error);
    void clearFields();

    bool isRegisterClicked(sf::Event& event, sf::RenderWindow& window);
    bool isBackLinkClicked(sf::Event& event, sf::RenderWindow& window);
};

#endif // REGISTERSCREEN_H
