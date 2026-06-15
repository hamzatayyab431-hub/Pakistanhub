#include "App.h"
#include "Theme.h"
#include <iostream>
#include <cstdlib>

App::App() : window(sf::VideoMode(1280, 720), "PakistanHub", sf::Style::Titlebar | sf::Style::Close),
             currentState(AppState::LOGIN), currentUser(nullptr),
             nextState(AppState::LOGIN), isTransitioning(false), transitionAlpha(0.0f) {
    window.setFramerateLimit(60);

    // Load typography font from assets
    if (!font.loadFromFile("assets/fonts/Roboto-Regular.ttf")) {
        std::cerr << "Error: Failed to load font assets/fonts/Roboto-Regular.ttf!\n";
        std::exit(1);
    }

    // Load persisted users, posts, follows, and comments
    if (!userManager.loadFromFile("data/users.txt")) {
        std::cerr << "Warning: Could not open data/users.txt, starting with empty database.\n";
    }
    if (!postManager.loadFromFile("data/posts.txt")) {
        std::cerr << "Warning: Could not open data/posts.txt, starting with empty database.\n";
    }
    if (!socialGraph.loadFromFile("data/follows.txt")) {
        std::cerr << "Warning: Could not open data/follows.txt, starting with empty graph.\n";
    }
    if (!commentManager.loadFromFile("data/comments.txt")) {
        std::cerr << "Warning: Could not open data/comments.txt, starting with empty database.\n";
    }

    // Initialize screen objects
    std::cout << "Creating LoginScreen..." << std::endl;
    loginScreen = std::make_unique<LoginScreen>(font);
    std::cout << "Creating RegisterScreen..." << std::endl;
    registerScreen = std::make_unique<RegisterScreen>(font);
    std::cout << "Creating FeedScreen..." << std::endl;
    feedScreen = std::make_unique<FeedScreen>(font, userManager, postManager, commentManager, socialGraph);
    std::cout << "Creating ProfileScreen..." << std::endl;
    profileScreen = std::make_unique<ProfileScreen>(font, userManager, postManager, commentManager, socialGraph);
    std::cout << "Creating PostDetailScreen..." << std::endl;
    postDetailScreen = std::make_unique<PostDetailScreen>(font, userManager, postManager, commentManager, socialGraph);
    std::cout << "Creating SearchScreen..." << std::endl;
    searchScreen = std::make_unique<SearchScreen>(font, userManager, postManager, socialGraph);
    std::cout << "App initialization complete." << std::endl;

    // Initialize background
    backgroundGradient.setPrimitiveType(sf::Quads);
    backgroundGradient.resize(4);
    backgroundGradient[0].position = sf::Vector2f(0.f, 0.f);
    backgroundGradient[0].color = Theme::BG_DARK;
    backgroundGradient[1].position = sf::Vector2f(1280.f, 0.f);
    backgroundGradient[1].color = Theme::BG_DARK;
    backgroundGradient[2].position = sf::Vector2f(1280.f, 720.f);
    backgroundGradient[2].color = Theme::BG_MID;
    backgroundGradient[3].position = sf::Vector2f(0.f, 720.f);
    backgroundGradient[3].color = Theme::BG_MID;

    glowTopLeft.setRadius(300.f);
    glowTopLeft.setPosition(-150.f, -150.f);
    glowTopLeft.setFillColor(sf::Color(0, 200, 100, 12));

    glowBottomRight.setRadius(250.f);
    glowBottomRight.setPosition(1280.f - 250.f, 720.f - 150.f);
    glowBottomRight.setFillColor(sf::Color(0, 200, 100, 12));

    glowCenter.setRadius(400.f);
    glowCenter.setPosition(1280.f / 2.f - 400.f, 720.f / 2.f - 400.f);
    glowCenter.setFillColor(sf::Color(255, 255, 255, 5));

    transitionOverlay.setSize(sf::Vector2f(1280.f, 720.f));
    transitionOverlay.setFillColor(sf::Color(0, 0, 0, 0));
}

void App::run() {
    while (window.isOpen()) {
        processEvents();
        update();
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

        // Delegate event handling based on the current AppState
        if (currentState == AppState::LOGIN) {
            loginScreen->handleEvent(event);

            // Check if REGISTER link is clicked
            if (loginScreen->isRegisterLinkClicked(event, window)) {
                loginScreen->clearFields();
                currentState = AppState::REGISTER;
            }
            // Check if LOGIN button is clicked
            else if (loginScreen->isLoginClicked(event, window)) {
                std::string username = loginScreen->getUsername();
                std::string password = loginScreen->getPassword();

                if (username.empty() || password.empty()) {
                    loginScreen->setErrorMessage("Please fill in all fields.");
                } else {
                    User* userPtr = userManager.loginUser(username, password);
                    if (userPtr != nullptr) {
                        currentUser = userPtr;
                        loginScreen->clearFields();
                        
                        // Set up and load user feed screen
                        feedScreen->setCurrentUser(currentUser);
                        feedScreen->reloadFeed();
                        
                        currentState = AppState::FEED;
                    } else {
                        loginScreen->setErrorMessage("Invalid username or password.");
                    }
                }
            }
        } 
        else if (currentState == AppState::REGISTER) {
            registerScreen->handleEvent(event);

            // Check if LOGIN link is clicked
            if (registerScreen->isBackLinkClicked(event, window)) {
                registerScreen->clearFields();
                currentState = AppState::LOGIN;
            }
            // Check if REGISTER button is clicked
            else if (registerScreen->isRegisterClicked(event, window)) {
                std::string username = registerScreen->getUsername();
                std::string displayName = registerScreen->getDisplayName();
                std::string password = registerScreen->getPassword();

                if (username.empty() || displayName.empty() || password.empty()) {
                    registerScreen->setErrorMessage("Please fill in all fields.");
                } else if (username.find('|') != std::string::npos || 
                           displayName.find('|') != std::string::npos || 
                           password.find('|') != std::string::npos) {
                    registerScreen->setErrorMessage("Disallowed character '|' entered.");
                } else {
                    bool success = userManager.registerUser(username, password, displayName);
                    if (success) {
                        registerScreen->clearFields();
                        loginScreen->setErrorMessage("Registration successful! Please login.");
                        currentState = AppState::LOGIN;
                    } else {
                        registerScreen->setErrorMessage("Username is already taken.");
                    }
                }
            }
        }
        else if (currentState == AppState::FEED) {
            feedScreen->handleEvent(event);

            if (feedScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                currentState = AppState::LOGIN;
            }
            else if (feedScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                currentState = AppState::PROFILE;
            }
            else if (feedScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch();
                currentState = AppState::SEARCH;
            }
            else if (feedScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
            }
            else {
                int postId = feedScreen->getClickedPostId();
                if (postId != -1) {
                    feedScreen->clearClickedPostId();
                    Post* targetPostPtr = nullptr;
                    for (const auto& p : postManager.getPosts()) {
                        if (p.getPostId() == postId) {
                            targetPostPtr = const_cast<Post*>(&p);
                            break;
                        }
                    }
                    if (targetPostPtr != nullptr) {
                        postDetailScreen->setCurrentUser(currentUser);
                        postDetailScreen->setTargetPost(*targetPostPtr, false);
                        currentState = AppState::POST_DETAIL;
                    }
                } else {
                    std::string clicked = feedScreen->getClickedHandle();
                    if (!clicked.empty()) {
                        feedScreen->clearClickedHandle();
                        User* target = userManager.findUser(clicked);
                        if (target != nullptr) {
                            profileScreen->setCurrentUser(currentUser);
                            profileScreen->setTargetUser(target);
                            currentState = AppState::PROFILE;
                        }
                    }
                }
            }
        }
        else if (currentState == AppState::PROFILE) {
            profileScreen->handleEvent(event);

            if (profileScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                currentState = AppState::LOGIN;
            }
            else if (profileScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
                currentState = AppState::FEED;
            }
            else if (profileScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch();
                currentState = AppState::SEARCH;
            }
            else if (profileScreen->isProfileClicked(event)) {
                profileScreen->setTargetUser(currentUser);
            }
            else {
                int postId = profileScreen->getClickedPostId();
                if (postId != -1) {
                    profileScreen->clearClickedPostId();
                    Post* targetPostPtr = nullptr;
                    for (const auto& p : postManager.getPosts()) {
                        if (p.getPostId() == postId) {
                            targetPostPtr = const_cast<Post*>(&p);
                            break;
                        }
                    }
                    if (targetPostPtr != nullptr) {
                        postDetailScreen->setCurrentUser(currentUser);
                        postDetailScreen->setTargetPost(*targetPostPtr, false);
                        currentState = AppState::POST_DETAIL;
                    }
                } else {
                    std::string clicked = profileScreen->getClickedHandle();
                    if (!clicked.empty()) {
                        profileScreen->clearClickedHandle();
                        User* target = userManager.findUser(clicked);
                        if (target != nullptr) {
                            profileScreen->setTargetUser(target);
                        }
                    }
                }
            }
        }
        else if (currentState == AppState::POST_DETAIL) {
            postDetailScreen->handleEvent(event);

            if (postDetailScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                currentState = AppState::LOGIN;
            }
            else if (postDetailScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
                currentState = AppState::FEED;
            }
            else if (postDetailScreen->isSearchClicked(event)) {
                searchScreen->setCurrentUser(currentUser);
                searchScreen->reloadSearch();
                currentState = AppState::SEARCH;
            }
            else if (postDetailScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                currentState = AppState::PROFILE;
            }
        }
        else if (currentState == AppState::SEARCH) {
            searchScreen->handleEvent(event);

            if (searchScreen->isLogoutClicked(event)) {
                currentUser = nullptr;
                currentState = AppState::LOGIN;
            }
            else if (searchScreen->isHomeClicked(event)) {
                feedScreen->reloadFeed();
                currentState = AppState::FEED;
            }
            else if (searchScreen->isProfileClicked(event)) {
                profileScreen->setCurrentUser(currentUser);
                profileScreen->setTargetUser(currentUser);
                currentState = AppState::PROFILE;
            }
            else if (searchScreen->isSearchClicked(event)) {
                searchScreen->reloadSearch();
            }
            else {
                std::string clicked = searchScreen->getClickedHandle();
                if (!clicked.empty()) {
                    searchScreen->clearClickedHandle();
                    User* target = userManager.findUser(clicked);
                    if (target != nullptr) {
                        profileScreen->setCurrentUser(currentUser);
                        profileScreen->setTargetUser(target);
                        currentState = AppState::PROFILE;
                    }
                }
            }
        }
    }
}

void App::update() {
    if (currentState == AppState::LOGIN) {
        loginScreen->update();
    } else if (currentState == AppState::REGISTER) {
        registerScreen->update();
    } else if (currentState == AppState::FEED) {
        feedScreen->update();
    } else if (currentState == AppState::PROFILE) {
        profileScreen->update();
    } else if (currentState == AppState::POST_DETAIL) {
        postDetailScreen->update();
    } else if (currentState == AppState::SEARCH) {
        searchScreen->update();
    }
}

void App::drawBackground() {
    window.draw(backgroundGradient);
    window.draw(glowTopLeft);
    window.draw(glowBottomRight);
    window.draw(glowCenter);
}

void App::transitionTo(AppState state) {
    currentState = state; // We'll implement actual fade logic in Redesign7
}

void App::render() {
    // Clear screen
    window.clear(Theme::BG_DARK);

    drawBackground();

    // Render active screen
    if (currentState == AppState::LOGIN) {
        loginScreen->draw(window);
    } else if (currentState == AppState::REGISTER) {
        registerScreen->draw(window);
    } else if (currentState == AppState::FEED) {
        feedScreen->draw(window);
    } else if (currentState == AppState::PROFILE) {
        profileScreen->draw(window);
    } else if (currentState == AppState::POST_DETAIL) {
        postDetailScreen->draw(window);
    } else if (currentState == AppState::SEARCH) {
        searchScreen->draw(window);
    }

    window.display();
}
