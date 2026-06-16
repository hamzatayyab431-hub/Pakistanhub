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
#include "Sidebar.h"
#include "RightPanel.h"
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
    User*           currentUser = nullptr;

    Sidebar    sidebar;
    RightPanel rightPanel;

    // ── Compose area ──────────────────────────────────────────────────────────
    std::unique_ptr<TextInput>   composeInput;
    std::unique_ptr<GlassButton> postButton;
    sf::Text                     charCountText;
    float                        composeHeight = 72.f;
    Lerp                         composeHeightLerp;

    // ── Tabs ──────────────────────────────────────────────────────────────────
    int   activeTab = 0;
    sf::Text tabText[3];
    sf::RectangleShape tabIndicator;
    Lerp   tabIndX;

    // ── Feed scroll ───────────────────────────────────────────────────────────
    sf::View feedViewport;
    float    scrollOffset     = 0.f;
    float    targetScrollOffset = 0.f;
    float    maxScrollOffset  = 0.f;

    sf::RectangleShape scrollTrack;
    sf::RectangleShape scrollThumb;

    // ── Empty state ───────────────────────────────────────────────────────────
    sf::Text emptyText;
    sf::Text emptySubText;

    std::vector<std::unique_ptr<PostCard>> postCards;
    std::string clickedHandle;
    int         clickedPostId = -1;

    // Layout helpers
    float feedX()  const { return Theme::CENTER_X + 0.f; }
    float feedW()  const { return Theme::CENTER_W; }
    float feedTop()const { return 0.f; }   // inside viewport

    void updateScrollbar();
    void updateCardPositions();

    // Viewport screen coords
    static constexpr float VP_X  = Theme::SIDEBAR_W;
    static constexpr float VP_Y  = 200.f;   // below compose + tabs
    static constexpr float VP_H  = 720.f - VP_Y;

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

    // Nav forwarding
    bool isHomeClicked(sf::Event& e)    { return sidebar.isClicked(e, SidebarItem::HOME); }
    bool isSearchClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::EXPLORE); }
    bool isProfileClicked(sf::Event& e) { return sidebar.isClicked(e, SidebarItem::PROFILE); }
    bool isLogoutClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::LOGOUT); }
};

#endif // FEEDSCREEN_H
