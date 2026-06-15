#ifndef FEEDSCREEN_H
#define FEEDSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "PostCard.h"
#include "PostManager.h"
#include "UserManager.h"
#include "SocialGraph.h"
#include "CommentManager.h"
#include <memory>
#include <vector>

class FeedScreen : public UIComponent {
private:
    sf::Font& font;
    UserManager& userManager;
    PostManager& postManager;
    CommentManager& commentManager;
    SocialGraph& socialGraph;
    User* currentUser;

    // Header shapes
    sf::RectangleShape headerBackground;
    sf::Text logoText;
    sf::Text statusText;
    std::unique_ptr<GlassButton> navHome;
    std::unique_ptr<GlassButton> navSearch;
    std::unique_ptr<GlassButton> navProfile;
    std::unique_ptr<GlassButton> navLogout;

    // Compose panel
    sf::RectangleShape composePanel;
    std::unique_ptr<TextInput> composeInput;
    std::unique_ptr<GlassButton> postButton;

    // Tabs UI
    int activeTab; // 0 = For You, 1 = Following
    sf::Text forYouText;
    sf::Text followingText;
    sf::RectangleShape tabIndicator;

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

    void updateCardPositions();

public:
    FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm, CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;

    void reloadFeed();
    void setCurrentUser(User* user);
    std::string getClickedHandle();
    void clearClickedHandle();
    int getClickedPostId();
    void clearClickedPostId();

    bool isHomeClicked(sf::Event& event);
    bool isSearchClicked(sf::Event& event);
    bool isProfileClicked(sf::Event& event);
    bool isLogoutClicked(sf::Event& event);
};

#endif // FEEDSCREEN_H
