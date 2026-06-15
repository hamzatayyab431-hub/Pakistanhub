#include "App.h"
#include "Theme.h"
#include "ToastManager.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

App::App()
    : window(sf::VideoMode(1280, 720), "PakistanHub", sf::Style::Titlebar | sf::Style::Close),
      currentState(AppState::LOGIN), currentUser(nullptr),
      nextState(AppState::LOGIN), isTransitioning(false), transitionAlpha(0.f)
{
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/fonts/Inter-Regular.ttf")) {
        std::cerr << "Error: Failed to load font assets/fonts/Inter-Regular.ttf!\n";
        std::exit(1);
    }

    if (!userManager.loadFromFile("data/users.txt"))
        std::cerr << "Warning: Could not open data/users.txt\n";
    if (!postManager.loadFromFile("data/posts.txt"))
        std::cerr << "Warning: Could not open data/posts.txt\n";
    if (!socialGraph.loadFromFile("data/follows.txt"))
        std::cerr << "Warning: Could not open data/follows.txt\n";
    if (!commentManager.loadFromFile("data/comments.txt"))
        std::cerr << "Warning: Could not open data/comments.txt\n";

    std::cout << "Creating screens..." << std::endl;
    loginScreen      = std::make_unique<LoginScreen>(font);
    registerScreen   = std::make_unique<RegisterScreen>(font);
    feedScreen       = std::make_unique<FeedScreen>(font, userManager, postManager, commentManager, socialGraph);
    profileScreen    = std::make_unique<ProfileScreen>(font, userManager, postManager, commentManager, socialGraph);
    postDetailScreen = std::make_unique<PostDetailScreen>(font, userManager, postManager, commentManager, socialGraph);
    searchScreen     = std::make_unique<SearchScreen>(font, userManager, postManager, socialGraph);
    std::cout << "App initialization complete." << std::endl;

    // Background gradient
    backgroundGradient.setPrimitiveType(sf::Quads);
    backgroundGradient.resize(4);
    backgroundGradient[0].position = sf::Vector2f(0.f, 0.f);
    backgroundGradient[0].color    = Theme::BG_BASE;
    backgroundGradient[1].position = sf::Vector2f(1280.f, 0.f);
    backgroundGradient[1].color    = Theme::BG_BASE;
    backgroundGradient[2].position = sf::Vector2f(1280.f, 720.f);
    backgroundGradient[2].color    = Theme::BG_MID;
    backgroundGradient[3].position = sf::Vector2f(0.f, 720.f);
    backgroundGradient[3].color    = Theme::BG_MID;

    glowTopLeft.setRadius(300.f);
    glowTopLeft.setPosition(-150.f, -150.f);
    glowTopLeft.setFillColor(Theme::GREEN_GLOW2);

    glowBottomRight.setRadius(250.f);
    glowBottomRight.setPosition(1030.f, 570.f);
    glowBottomRight.setFillColor(Theme::GREEN_GLOW2);

    glowCenter.setRadius(400.f);
    glowCenter.setPosition(240.f, -80.f);
    glowCenter.setFillColor(sf::Color(255, 255, 255, 4));

    transitionOverlay.setSize(sf::Vector2f(1280.f, 720.f));
    transitionOverlay.setFillColor(sf::Color(0, 0, 0, 0));
}

void App::run() {
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;   // cap at 50ms

        processEvents();
        update(dt);
        render();
    }
}

void App::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }

        if (isTransitioning) continue;

        if (currentState == AppState::LOGIN) {
            loginScreen->handleEvent(event);

            if (loginScreen->isRegisterLinkClicked(event, window)) {
                loginScreen->clearFields();
                transitionTo(AppState::REGISTER);
            } else if (loginScreen->isLoginClicked(event, window)) {
                std::string user = loginScreen->getUsername();
                std::string pass = loginScreen->getPassword();

                if (user.empty() || pass.empty()) {
                    loginScreen->setErrorMessage("Please fill in all fields.");
                    ToastManager::instance().push("Login failed", "Fill in all fields", ToastType::ERROR);
                } else {
                    User* uptr = userManager.loginUser(user, pass);
                    if (uptr) {
                        currentUser = uptr;
                        loginScreen->clearFields();
                        feedScreen->setCurrentUser(currentUser);
                        feedScreen->reloadFeed();
                        transitionTo(AppState::FEED);
                    } else {
                        loginScreen->setErrorMessage("Invalid username or password.");
                        ToastManager::instance().push("Login failed", "Check your credentials", ToastType::ERROR);
                    }
                }
            }
        }
        else if (currentState == AppState::REGISTER) {
            registerScreen->handleEvent(event);

            if (registerScreen->isBackLinkClicked(event, window)) {
                registerScreen->clearFields();
                transitionTo(AppState::LOGIN);
            } else if (registerScreen->isRegisterClicked(event, window)) {
                std::string uname   = registerScreen->getUsername();
                std::string dname   = registerScreen->getDisplayName();
                std::string bio     = registerScreen->getBio();
                std::string city    = registerScreen->getCity();
                std::string pass    = registerScreen->getPassword();
                std::string confirm = registerScreen->getConfirmPassword();

                if (uname.empty() || dname.empty() || pass.empty() || confirm.empty()) {
                    registerScreen->setErrorMessage("Please fill in all fields.");
                } else if (pass != confirm) {
                    registerScreen->setErrorMessage("Passwords do not match.");
                } else if (uname.find('|') != std::string::npos ||
                           dname.find('|') != std::string::npos ||
                           pass.find('|')  != std::string::npos) {
                    registerScreen->setErrorMessage("Disallowed character '|' entered.");
                } else {
                    bool ok = userManager.registerUser(uname, pass, dname, bio, city);
                    if (ok) {
                        registerScreen->clearFields();
                        loginScreen->setErrorMessage("Registration successful! Please login.");
                        ToastManager::instance().push("Account created!", "Welcome to PakistanHub", ToastType::SUCCESS);
                        transitionTo(AppState::LOGIN);
                    } else {
                        registerScreen->setErrorMessage("Username is already taken.");
                        ToastManager::instance().push("Username taken", "Try a different username", ToastType::ERROR);
                    }
                }
            }
        }
        else if (currentState == AppState::FEED) {
            feedScreen->handleEvent(event);

            if (feedScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                transitionTo(AppState::LOGIN);
            } else if (feedScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                transitionTo(AppState::PROFILE);
            } else if (feedScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch();
                transitionTo(AppState::SEARCH);
            } else if (feedScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
            } else {
                int pid = feedScreen->getClickedPostId();
                if (pid != -1) {
                    feedScreen->clearClickedPostId();
                    for (const auto& p : postManager.getPosts()) {
                        if (p.getPostId() == pid) {
                            postDetailScreen->setCurrentUser(currentUser);
                            postDetailScreen->setTargetPost(p, false);
                            transitionTo(AppState::POST_DETAIL);
                            break;
                        }
                    }
                } else {
                    std::string hdl = feedScreen->getClickedHandle();
                    if (!hdl.empty()) {
                        feedScreen->clearClickedHandle();
                        User* tgt = userManager.findUser(hdl);
                        if (tgt) {
                            profileScreen->setCurrentUser(currentUser);
                            profileScreen->setTargetUser(tgt);
                            transitionTo(AppState::PROFILE);
                        }
                    }
                }
            }
        }
        else if (currentState == AppState::PROFILE) {
            profileScreen->handleEvent(event);

            if (profileScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                transitionTo(AppState::LOGIN);
            } else if (profileScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
                transitionTo(AppState::FEED);
            } else if (profileScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch();
                transitionTo(AppState::SEARCH);
            } else if (profileScreen->isProfileClicked(event)) {
                profileScreen->setTargetUser(currentUser);
            } else {
                int pid = profileScreen->getClickedPostId();
                if (pid != -1) {
                    profileScreen->clearClickedPostId();
                    for (const auto& p : postManager.getPosts()) {
                        if (p.getPostId() == pid) {
                            postDetailScreen->setCurrentUser(currentUser);
                            postDetailScreen->setTargetPost(p, false);
                            transitionTo(AppState::POST_DETAIL);
                            break;
                        }
                    }
                } else {
                    std::string hdl = profileScreen->getClickedHandle();
                    if (!hdl.empty()) {
                        profileScreen->clearClickedHandle();
                        User* tgt = userManager.findUser(hdl);
                        if (tgt) profileScreen->setTargetUser(tgt);
                    }
                }
            }
        }
        else if (currentState == AppState::POST_DETAIL) {
            postDetailScreen->handleEvent(event);

            if (postDetailScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                transitionTo(AppState::LOGIN);
            } else if (postDetailScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
                transitionTo(AppState::FEED);
            } else if (postDetailScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch();
                transitionTo(AppState::SEARCH);
            } else if (postDetailScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                transitionTo(AppState::PROFILE);
            }
        }
        else if (currentState == AppState::SEARCH) {
            searchScreen->handleEvent(event);

            if (searchScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                transitionTo(AppState::LOGIN);
            } else if (searchScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
                transitionTo(AppState::FEED);
            } else if (searchScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                transitionTo(AppState::PROFILE);
            } else if (searchScreen->isSearchClicked(event)) {
                searchScreen->reloadSearch();
            } else {
                std::string hdl = searchScreen->getClickedHandle();
                if (!hdl.empty()) {
                    searchScreen->clearClickedHandle();
                    User* tgt = userManager.findUser(hdl);
                    if (tgt) {
                        profileScreen->setCurrentUser(currentUser);
                        profileScreen->setTargetUser(tgt);
                        transitionTo(AppState::PROFILE);
                    }
                }
            }
        }
    }
}

void App::update(float dt) {
    float time = appClock.getElapsedTime().asSeconds();

    // Animate ambient glows
    glowTopLeft.setPosition(
        -150.f + std::sin(time * 0.5f) * 30.f,
        -150.f + std::cos(time * 0.3f) * 30.f);
    glowBottomRight.setPosition(
        1030.f + std::cos(time * 0.4f) * 40.f,
        570.f  + std::sin(time * 0.6f) * 40.f);
    glowCenter.setPosition(
        240.f + std::sin(time * 0.2f) * 50.f,
        -80.f + std::cos(time * 0.25f) * 50.f);

    // Screen transition
    if (isTransitioning) {
        if (currentState != nextState) {
            transitionAlpha += 14.f * dt * 60.f;
            if (transitionAlpha >= 210.f) {
                transitionAlpha = 210.f;
                currentState = nextState;
            }
        } else {
            transitionAlpha -= 14.f * dt * 60.f;
            if (transitionAlpha <= 0.f) {
                transitionAlpha = 0.f;
                isTransitioning = false;
            }
        }
        transitionOverlay.setFillColor(
            sf::Color(Theme::BG_BASE.r, Theme::BG_BASE.g, Theme::BG_BASE.b,
                      static_cast<sf::Uint8>(transitionAlpha)));
    }

    // Update active screen with delta time
    switch (currentState) {
        case AppState::LOGIN:       loginScreen->update(dt);      break;
        case AppState::REGISTER:    registerScreen->update(dt);   break;
        case AppState::FEED:        feedScreen->update(dt);       break;
        case AppState::PROFILE:     profileScreen->update(dt);    break;
        case AppState::POST_DETAIL: postDetailScreen->update(dt); break;
        case AppState::SEARCH:      searchScreen->update(dt);     break;
    }

    ToastManager::instance().update(dt);
}

void App::drawBackground() {
    window.draw(backgroundGradient);
    window.draw(glowTopLeft);
    window.draw(glowBottomRight);
    window.draw(glowCenter);
}

void App::transitionTo(AppState state) {
    if (state != currentState) {
        nextState = state;
        isTransitioning = true;
    }
}

void App::render() {
    window.clear(Theme::BG_BASE);
    drawBackground();

    switch (currentState) {
        case AppState::LOGIN:       loginScreen->draw(window);      break;
        case AppState::REGISTER:    registerScreen->draw(window);   break;
        case AppState::FEED:        feedScreen->draw(window);       break;
        case AppState::PROFILE:     profileScreen->draw(window);    break;
        case AppState::POST_DETAIL: postDetailScreen->draw(window); break;
        case AppState::SEARCH:      searchScreen->draw(window);     break;
    }

    ToastManager::instance().draw(window, font);

    if (isTransitioning) window.draw(transitionOverlay);

    window.display();
}
