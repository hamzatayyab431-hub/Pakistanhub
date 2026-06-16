#ifndef PROFILESCREEN_H
#define PROFILESCREEN_H

#include "UIComponent.h"
#include "GlassButton.h"
#include "TextInput.h"
#include "PostCard.h"
#include "PostManager.h"
#include "UserManager.h"
#include "SocialGraph.h"
#include "CommentManager.h"
#include "Sidebar.h"
#include "Animator.h"
#include <memory>
#include <vector>

class ProfileScreen : public UIComponent {
private:
    sf::Font&       font;
    UserManager&    userManager;
    PostManager&    postManager;
    CommentManager& commentManager;
    SocialGraph&    socialGraph;
    User*           currentUser = nullptr;
    User*           targetUser  = nullptr;

    Sidebar sidebar;

    // Cover + avatar area
    sf::RectangleShape coverBg;
    sf::Text           nameText;
    sf::Text           handleText;
    sf::Text           bioText;
    sf::Text           cityText;
    sf::Text           statsText;

    std::unique_ptr<GlassButton> followButton;
    std::unique_ptr<GlassButton> editProfileButton;
    std::unique_ptr<GlassButton> saveProfileButton;
    std::unique_ptr<TextInput>   cityEditInput;
    std::unique_ptr<TextInput>   bioEditInput;
    bool isEditingProfile = false;

    // Profile tabs
    sf::Text       tabText[3];
    sf::RectangleShape tabIndicator;
    Lerp           tabIndX;
    int            activeProfileTab = 0;

    // Scrollable posts
    sf::View       feedViewport;
    float          scrollOffset      = 0.f;
    float          targetScrollOffset = 0.f;
    float          maxScrollOffset   = 0.f;

    sf::RectangleShape scrollTrack;
    sf::RectangleShape scrollThumb;

    sf::Text emptyText;

    std::vector<std::unique_ptr<PostCard>> postCards;
    std::string clickedHandle;
    int         clickedPostId = -1;

    static constexpr float CX     = Theme::SIDEBAR_W;
    static constexpr float CW     = Theme::CENTER_W;
    static constexpr float COVER_H = 130.f;
    static constexpr float HEADER_H = COVER_H + 90.f;  // cover + info below
    static constexpr float TAB_Y   = HEADER_H + 8.f;
    static constexpr float VP_Y    = TAB_Y + 36.f;
    static constexpr float VP_H    = 720.f - VP_Y;

public:
    ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                  CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override { update(0.016f); }
    void update(float dt);

    void setTargetUser(User* user);
    void setCurrentUser(User* user);
    void reloadProfile();

    bool isBackClicked(sf::Event& event, sf::RenderWindow& window);
    bool isHomeClicked(sf::Event& e)    { return sidebar.isClicked(e, SidebarItem::HOME); }
    bool isSearchClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::EXPLORE); }
    bool isProfileClicked(sf::Event& e) { return sidebar.isClicked(e, SidebarItem::PROFILE); }
    bool isLogoutClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::LOGOUT); }

    std::string getClickedHandle();
    void        clearClickedHandle();
    int         getClickedPostId();
    void        clearClickedPostId();
};

#endif // PROFILESCREEN_H
