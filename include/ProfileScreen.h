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
#include "NavBar.h"
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
    User*           currentUser;
    User*           targetUser;

    NavBar nav;

    // Profile header card
    sf::RectangleShape headerCard;
    sf::Text nameText;
    sf::Text handleText;
    sf::Text bioText;
    sf::Text cityText;
    sf::Text statsText;

    std::unique_ptr<GlassButton> followButton;
    std::unique_ptr<GlassButton> editProfileButton;
    std::unique_ptr<GlassButton> saveProfileButton;
    std::unique_ptr<TextInput>   cityEditInput;
    std::unique_ptr<TextInput>   bioEditInput;
    bool isEditingProfile;

    // Scrollable feed
    sf::View       feedViewport;
    float          scrollOffset;
    float          targetScrollOffset;
    float          maxScrollOffset;

    sf::RectangleShape scrollTrack;
    sf::RectangleShape scrollThumb;

    sf::Text emptyText;

    std::vector<std::unique_ptr<PostCard>> postCards;
    std::string clickedHandle;
    int         clickedPostId;

public:
    ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                  CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;
    void update(float dt);

    void setTargetUser(User* user);
    void setCurrentUser(User* user);
    void reloadProfile();

    bool isBackClicked(sf::Event& event, sf::RenderWindow& window);
    bool isHomeClicked(sf::Event& event);
    bool isSearchClicked(sf::Event& event);
    bool isProfileClicked(sf::Event& event);
    bool isLogoutClicked(sf::Event& event);

    std::string getClickedHandle();
    void        clearClickedHandle();
    int         getClickedPostId();
    void        clearClickedPostId();
};

#endif // PROFILESCREEN_H
