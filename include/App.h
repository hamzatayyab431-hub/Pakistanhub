#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>
#include "UserManager.h"
#include "PostManager.h"
#include "SocialGraph.h"
#include "CommentManager.h"
#include "LoginScreen.h"
#include "RegisterScreen.h"
#include "FeedScreen.h"
#include "ProfileScreen.h"
#include "PostDetailScreen.h"
#include "SearchScreen.h"
#include "ToastManager.h"
#include "Animator.h"
#include <memory>

class App {
public:
    enum class AppState { LOGIN, REGISTER, FEED, PROFILE, POST_DETAIL, SEARCH };

private:
    sf::RenderWindow window;
    AppState currentState;

    sf::Font font;
    UserManager    userManager;
    PostManager    postManager;
    SocialGraph    socialGraph;
    CommentManager commentManager;
    User* currentUser = nullptr;

    std::unique_ptr<LoginScreen>      loginScreen;
    std::unique_ptr<RegisterScreen>   registerScreen;
    std::unique_ptr<FeedScreen>       feedScreen;
    std::unique_ptr<ProfileScreen>    profileScreen;
    std::unique_ptr<PostDetailScreen> postDetailScreen;
    std::unique_ptr<SearchScreen>     searchScreen;

    sf::VertexArray backgroundGradient;
    sf::Clock       appClock;
    sf::Clock       frameClock;

    AppState nextState;
    bool     isTransitioning  = false;
    float    transitionAlpha  = 0.f;
    sf::RectangleShape transitionOverlay;

    void processEvents();
    void update(float dt);
    void render();
    void drawBackground();
    void transitionTo(AppState state);

public:
    App();
    void run();
};

#endif // APP_H
