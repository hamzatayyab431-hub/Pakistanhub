#include "App.h"

App::App() : window(sf::VideoMode(1280, 720), "PakistanHub", sf::Style::Titlebar | sf::Style::Close) {
    window.setFramerateLimit(60);
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
        }
    }
}

void App::update() {
    // Phase 1: Update logic empty
}

void App::render() {
    // Clear screen with deep dark background (#0D1117)
    window.clear(sf::Color(0x0D, 0x11, 0x17));

    // Phase 1: Render logic empty

    window.display();
}
