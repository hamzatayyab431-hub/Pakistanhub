#ifndef PROFILESCREEN_H
#define PROFILESCREEN_H

#include "UIComponent.h"
#include "GlassButton.h"
#include "PostCard.h"
#include "PostManager.h"
#include "UserManager.h"
#include "SocialGraph.h"
#include <memory>
#include <vector>

class ProfileScreen : public UIComponent {
private:
    sf::Font& font;
    UserManager& userManager;
    PostManager& postManager;
    SocialGraph& socialGraph;
    User* currentUser;
    User* targetUser;

    // UI elements
    sf::Text backLink;

    // Profile Header card
    sf::RectangleShape headerCard;
    sf::Text nameText;
    sf::Text handleText;
    sf::Text bioText;
    sf::Text statsText;
    std::unique_ptr<GlassButton> followButton;

    // Scrollable feed viewport
    sf::View feedViewport;
    float scrollOffset;
    float maxScrollOffset;

    std::vector<std::unique_ptr<PostCard>> postCards;
    std::string clickedHandle;

public:
    ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    void setTargetUser(User* user);
    void setCurrentUser(User* user);
    void reloadProfile();

    bool isBackClicked(sf::Event& event, sf::RenderWindow& window);
    std::string getClickedHandle();
    void clearClickedHandle();
};

#endif // PROFILESCREEN_H
