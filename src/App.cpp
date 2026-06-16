#include "App.h"
#include "Theme.h"
#include "ToastManager.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

App::App()
    : window(sf::VideoMode(1280, 720), "PakistanHub", sf::Style::Titlebar | sf::Style::Close),
      currentState(AppState::LOGIN),
      nextState(AppState::LOGIN)
{
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/fonts/Inter-Regular.ttf")) {
        std::cerr << "Error: Failed to load Inter-Regular.ttf\n";
        std::exit(1);
    }

    if (!userManager.loadFromFile("data/users.txt"))
        std::cerr << "Warning: data/users.txt not found\n";
    if (!postManager.loadFromFile("data/posts.txt"))
        std::cerr << "Warning: data/posts.txt not found\n";
    if (!socialGraph.loadFromFile("data/follows.txt"))
        std::cerr << "Warning: data/follows.txt not found\n";
    if (!commentManager.loadFromFile("data/comments.txt"))
        std::cerr << "Warning: data/comments.txt not found\n";

    loginScreen      = std::make_unique<LoginScreen>(font);
    registerScreen   = std::make_unique<RegisterScreen>(font);
    feedScreen       = std::make_unique<FeedScreen>(font, userManager, postManager, commentManager, socialGraph);
    profileScreen    = std::make_unique<ProfileScreen>(font, userManager, postManager, commentManager, socialGraph);
    postDetailScreen = std::make_unique<PostDetailScreen>(font, userManager, postManager, commentManager, socialGraph);
    searchScreen     = std::make_unique<SearchScreen>(font, userManager, postManager, socialGraph);

    // Simple gradient background
    backgroundGradient.setPrimitiveType(sf::Quads);
    backgroundGradient.resize(4);
    backgroundGradient[0].position = {0.f, 0.f};     backgroundGradient[0].color = Theme::BG_PRIMARY;
    backgroundGradient[1].position = {1280.f, 0.f};  backgroundGradient[1].color = Theme::BG_PRIMARY;
    backgroundGradient[2].position = {1280.f, 720.f};backgroundGradient[2].color = Theme::BG_SECONDARY;
    backgroundGradient[3].position = {0.f, 720.f};   backgroundGradient[3].color = Theme::BG_SECONDARY;

    transitionOverlay.setSize({1280.f, 720.f});
    transitionOverlay.setFillColor(sf::Color::Transparent);
}

void App::run() {
    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        processEvents();
        update(dt);
        render();
    }
}

void App::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) { window.close(); return; }
        if (isTransitioning) continue;

        if (currentState == AppState::LOGIN) {
            loginScreen->handleEvent(event);
            if (loginScreen->isRegisterLinkClicked(event, window)) {
                loginScreen->clearFields();
                transitionTo(AppState::REGISTER);
            } else if (loginScreen->isLoginClicked(event, window)) {
                std::string u = loginScreen->getUsername();
                std::string p = loginScreen->getPassword();
                if (u.empty() || p.empty()) {
                    loginScreen->setErrorMessage("Please fill in all fields.");
                    ToastManager::instance().push("Login failed","Fill in all fields",ToastType::ERROR);
                } else {
                    User* uptr = userManager.loginUser(u, p);
                    if (uptr) {
                        currentUser = uptr;
                        loginScreen->clearFields();
                        feedScreen->setCurrentUser(currentUser);
                        feedScreen->reloadFeed();
                        transitionTo(AppState::FEED);
                    } else {
                        loginScreen->setErrorMessage("Invalid username or password.");
                        ToastManager::instance().push("Login failed","Check your credentials",ToastType::ERROR);
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
                std::string un = registerScreen->getUsername();
                std::string dn = registerScreen->getDisplayName();
                std::string bio= registerScreen->getBio();
                std::string cy = registerScreen->getCity();
                std::string pw = registerScreen->getPassword();
                std::string cp = registerScreen->getConfirmPassword();
                if (un.empty()||dn.empty()||pw.empty()||cp.empty()) {
                    registerScreen->setErrorMessage("Please fill in all fields.");
                } else if (pw != cp) {
                    registerScreen->setErrorMessage("Passwords do not match.");
                } else if (un.find('|')!=std::string::npos||dn.find('|')!=std::string::npos||pw.find('|')!=std::string::npos) {
                    registerScreen->setErrorMessage("Disallowed character '|'.");
                } else {
                    if (userManager.registerUser(un,pw,dn,bio,cy)) {
                        registerScreen->clearFields();
                        loginScreen->setErrorMessage("Registration successful! Please login.");
                        ToastManager::instance().push("Account created!","Welcome to PakistanHub",ToastType::SUCCESS);
                        transitionTo(AppState::LOGIN);
                    } else {
                        registerScreen->setErrorMessage("Username is already taken.");
                        ToastManager::instance().push("Username taken","Try a different username",ToastType::ERROR);
                    }
                }
            }
        }
        else if (currentState == AppState::FEED) {
            feedScreen->handleEvent(event);
            if (feedScreen->isLogoutClicked(event)) {
                currentUser = nullptr; transitionTo(AppState::LOGIN);
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
                currentUser = nullptr; transitionTo(AppState::LOGIN);
            } else if (profileScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed(); transitionTo(AppState::FEED);
            } else if (profileScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch(); transitionTo(AppState::SEARCH);
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
                currentUser = nullptr; transitionTo(AppState::LOGIN);
            } else if (postDetailScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed(); transitionTo(AppState::FEED);
            } else if (postDetailScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch(); transitionTo(AppState::SEARCH);
            } else if (postDetailScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                transitionTo(AppState::PROFILE);
            }
        }
        else if (currentState == AppState::SEARCH) {
            searchScreen->handleEvent(event);
            if (searchScreen->isLogoutClicked(event)) {
                currentUser = nullptr; transitionTo(AppState::LOGIN);
            } else if (searchScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed(); transitionTo(AppState::FEED);
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
    if (isTransitioning) {
        if (currentState != nextState) {
            transitionAlpha += 14.f * dt * 60.f;
            if (transitionAlpha >= 200.f) { transitionAlpha = 200.f; currentState = nextState; }
        } else {
            transitionAlpha -= 14.f * dt * 60.f;
            if (transitionAlpha <= 0.f) { transitionAlpha = 0.f; isTransitioning = false; }
        }
        transitionOverlay.setFillColor(sf::Color(
            Theme::BG_PRIMARY.r, Theme::BG_PRIMARY.g, Theme::BG_PRIMARY.b,
            static_cast<sf::Uint8>(transitionAlpha)));
    }

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
}

void App::transitionTo(AppState state) {
    if (state != currentState) { nextState = state; isTransitioning = true; }
}

void App::render() {
    window.clear(Theme::BG_PRIMARY);
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
