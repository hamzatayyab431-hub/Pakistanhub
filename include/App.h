#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include "UserManager.h"
#include "PostManager.h"
#include "SocialGraph.h"
#include "LoginScreen.h"
#include "RegisterScreen.h"
#include "FeedScreen.h"
#include "ProfileScreen.h"
#include <memory>

class App {
public:
    enum class AppState { LOGIN, REGISTER, FEED, PROFILE };

private:
    sf::RenderWindow window;
    AppState currentState;

    sf::Font font;
    UserManager userManager;
    PostManager postManager;
    SocialGraph socialGraph;
    User* currentUser;

    std::unique_ptr<LoginScreen> loginScreen;
    std::unique_ptr<RegisterScreen> registerScreen;
    std::unique_ptr<FeedScreen> feedScreen;
    std::unique_ptr<ProfileScreen> profileScreen;

    void processEvents();
    void update();
    void render();

public:
    App();
    void run();
};

#endif // APP_H
