#include "App.h"
#include <iostream>

App::App() : window(sf::VideoMode(1280, 720), "PakistanHub", sf::Style::Titlebar | sf::Style::Close),
             currentState(AppState::LOGIN), currentUser(nullptr) {
    window.setFramerateLimit(60);

    // Load typography font from assets
    if (!font.loadFromFile("assets/fonts/Roboto-Regular.ttf")) {
        std::cerr << "Error: Failed to load font assets/fonts/Roboto-Regular.ttf!\n";
    }

    // Load persisted users and posts
    if (!userManager.loadFromFile("data/users.txt")) {
        std::cerr << "Warning: Could not open data/users.txt, starting with empty database.\n";
    }
    if (!postManager.loadFromFile("data/posts.txt")) {
        std::cerr << "Warning: Could not open data/posts.txt, starting with empty database.\n";
    }

    // Initialize screen objects
    loginScreen = std::make_unique<LoginScreen>(font);
    registerScreen = std::make_unique<RegisterScreen>(font);
    feedScreen = std::make_unique<FeedScreen>(font, userManager, postManager);
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

            // Check if logout was clicked
            if (feedScreen->isLogoutClicked(event, window)) {
                currentUser = nullptr;
                currentState = AppState::LOGIN;
            }
        }
    }
}

void App::update() {
    // AppState::FEED updates are handled inside FeedScreen
}

void App::render() {
    // Clear screen with deep dark background (#0D1117)
    window.clear(sf::Color(0x0D, 0x11, 0x17));

    // Render active screen
    if (currentState == AppState::LOGIN) {
        loginScreen->draw(window);
    } else if (currentState == AppState::REGISTER) {
        registerScreen->draw(window);
    } else if (currentState == AppState::FEED) {
        feedScreen->draw(window);
    }

    window.display();
}
