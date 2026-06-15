#ifndef PROFILESCREEN_H
#define PROFILESCREEN_H

#include "UIComponent.h"
#include "GlassButton.h"
#include "PostCard.h"
#include "PostManager.h"
#include "UserManager.h"
#include "SocialGraph.h"
#include "CommentManager.h"
#include <memory>
#include <vector>

class ProfileScreen : public UIComponent {
private:
    sf::Font& font;
    UserManager& userManager;
    PostManager& postManager;
    CommentManager& commentManager;
    SocialGraph& socialGraph;
    User* currentUser;
    User* targetUser;

    // Persistent Top Navigation Bar
    sf::RectangleShape headerBackground;
    sf::Text logoText;
    sf::Text statusText;
    std::unique_ptr<GlassButton> navHome;
    std::unique_ptr<GlassButton> navSearch;
    std::unique_ptr<GlassButton> navProfile;
    std::unique_ptr<GlassButton> navLogout;

    // Profile Header card
    sf::RectangleShape headerCard;
    sf::CircleShape avatarCircle;
    sf::Text avatarLetter;
    sf::RectangleShape navAccentLine;
    sf::Text nameText;
    sf::Text handleText;
    sf::Text bioText;
    sf::Text cityText;
    sf::Text statsText;
    std::unique_ptr<GlassButton> followButton;

    // Scrollable feed viewport
    sf::View feedViewport;
    float scrollOffset;
    float targetScrollOffset;
    float maxScrollOffset;

    // Empty state panel
    sf::RectangleShape emptyCard;
    sf::Text emptyText;

    std::vector<std::unique_ptr<PostCard>> postCards;
    std::string clickedHandle;
    int clickedPostId;

public:
    ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm, CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;

    void setTargetUser(User* user);
    void setCurrentUser(User* user);
    void reloadProfile();

    bool isBackClicked(sf::Event& event, sf::RenderWindow& window);
    bool isHomeClicked(sf::Event& event);
    bool isSearchClicked(sf::Event& event);
    bool isProfileClicked(sf::Event& event);
    bool isLogoutClicked(sf::Event& event);
    std::string getClickedHandle();
    void clearClickedHandle();
    int getClickedPostId();
    void clearClickedPostId();
};

#endif // PROFILESCREEN_H
