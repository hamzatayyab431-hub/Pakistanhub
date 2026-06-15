#ifndef APP_H
#define APP_H

#include <SFML/Graphics.hpp>

class App {
private:
    sf::RenderWindow window;

    void processEvents();
    void update();
    void render();

public:
    App();
    void run();
};

#endif // APP_H
