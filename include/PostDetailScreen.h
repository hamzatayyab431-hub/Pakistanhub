#ifndef POSTDETAILSCREEN_H
#define POSTDETAILSCREEN_H

#include "UIComponent.h"
#include "PostCard.h"
#include "CommentCard.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "PostManager.h"
#include "UserManager.h"
#include "CommentManager.h"
#include "SocialGraph.h"
#include "Sidebar.h"
#include <memory>
#include <vector>

class PostDetailScreen : public UIComponent {
private:
    sf::Font&       font;
    UserManager&    userManager;
    PostManager&    postManager;
    CommentManager& commentManager;
    SocialGraph&    socialGraph;
    User*           currentUser = nullptr;
    Post            targetPost;
    bool            targetPostLiked = false;

    Sidebar sidebar;

    std::unique_ptr<PostCard> postCard;

    sf::View commentViewport;
    float    scrollOffset      = 0.f;
    float    targetScrollOffset = 0.f;
    float    maxScrollOffset   = 0.f;

    std::vector<std::unique_ptr<CommentCard>> commentCards;

    sf::RectangleShape           replyBackground;
    std::unique_ptr<TextInput>   replyInput;
    std::unique_ptr<GlassButton> replyButton;

    static constexpr float CX     = Theme::SIDEBAR_W;
    static constexpr float CW     = Theme::CENTER_W;

public:
    PostDetailScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                     CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override { update(0.016f); }
    void update(float dt);

    void setTargetPost(const Post& post, bool liked);
    void setCurrentUser(User* user);
    void reloadComments();

    bool isHomeClicked(sf::Event& e)    { return sidebar.isClicked(e, SidebarItem::HOME); }
    bool isSearchClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::EXPLORE); }
    bool isProfileClicked(sf::Event& e) { return sidebar.isClicked(e, SidebarItem::PROFILE); }
    bool isLogoutClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::LOGOUT); }
};

#endif // POSTDETAILSCREEN_H
