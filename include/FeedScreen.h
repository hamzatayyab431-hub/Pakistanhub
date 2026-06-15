#ifndef FEEDSCREEN_H
#define FEEDSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "PostCard.h"
#include "PostManager.h"
#include "UserManager.h"
#include <memory>
#include <vector>

class FeedScreen : public UIComponent {
private:
    sf::Font& font;
    UserManager& userManager;
    PostManager& postManager;
    User* currentUser;

    // Header shapes
    sf::RectangleShape headerBackground;
    sf::Text logoText;
    sf::Text profileText;
    sf::Text logoutText;

    // Compose panel
    sf::RectangleShape composePanel;
    std::unique_ptr<TextInput> composeInput;
    std::unique_ptr<GlassButton> postButton;

    // Scrollable feed viewport
    sf::View feedViewport;
    float scrollOffset;
    float maxScrollOffset;

    std::vector<std::unique_ptr<PostCard>> postCards;

    void updateCardPositions();

public:
    FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    void reloadFeed();
    void setCurrentUser(User* user);

    bool isLogoutClicked(sf::Event& event, sf::RenderWindow& window);
};

#endif // FEEDSCREEN_H
