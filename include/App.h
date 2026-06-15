#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include "UserManager.h"
#include "LoginScreen.h"
#include "RegisterScreen.h"
#include <memory>

class App {
public:
    enum class AppState { LOGIN, REGISTER, FEED };

private:
    sf::RenderWindow window;
    AppState currentState;

    sf::Font font;
    UserManager userManager;
    User* currentUser;

    std::unique_ptr<LoginScreen> loginScreen;
    std::unique_ptr<RegisterScreen> registerScreen;

    sf::Text feedTempText;

    void processEvents();
    void update();
    void render();

public:
    App();
    void run();
};

#endif // APP_H
