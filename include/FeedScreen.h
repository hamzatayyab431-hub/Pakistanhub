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
#include "NavBar.h"
#include "Animator.h"
#include <memory>
#include <vector>

class FeedScreen : public UIComponent {
private:
    sf::Font&       font;
    UserManager&    userManager;
    PostManager&    postManager;
    CommentManager& commentManager;
    SocialGraph&    socialGraph;
    User*           currentUser;

    NavBar nav;

    // Compose
    sf::RectangleShape composePanel;
    std::unique_ptr<TextInput>   composeInput;
    std::unique_ptr<GlassButton> postButton;
    sf::Text                     charCountText;
    float                        composeHeight = 90.f;
    Lerp                         composeHeightLerp;

    // Tabs
    int            activeTab = 0;
    sf::Text       tabText[3];
    sf::RectangleShape tabIndicator;
    Lerp           tabIndX;

    // Scrollable feed
    sf::View       feedViewport;
    float          scrollOffset;
    float          targetScrollOffset;
    float          maxScrollOffset;

    sf::RectangleShape scrollTrack;
    sf::RectangleShape scrollThumb;

    // Empty state
    sf::Text emptyText;
    sf::Text emptySubText;

    std::vector<std::unique_ptr<PostCard>> postCards;
    std::string clickedHandle;
    int         clickedPostId;

    void updateCardPositions();
    void updateScrollbar();

public:
    FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
               CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override { update(0.016f); }
    void update(float dt);

    void reloadFeed();
    void setCurrentUser(User* user);
    std::string getClickedHandle();
    void        clearClickedHandle();
    int         getClickedPostId();
    void        clearClickedPostId();

    bool isHomeClicked(sf::Event& event);
    bool isSearchClicked(sf::Event& event);
    bool isProfileClicked(sf::Event& event);
    bool isLogoutClicked(sf::Event& event);
};

#endif // FEEDSCREEN_H
