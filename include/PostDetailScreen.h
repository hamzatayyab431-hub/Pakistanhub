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
#include "NavBar.h"
#include <memory>
#include <vector>

class PostDetailScreen : public UIComponent {
private:
    sf::Font&       font;
    UserManager&    userManager;
    PostManager&    postManager;
    CommentManager& commentManager;
    SocialGraph&    socialGraph;
    User*           currentUser;
    Post            targetPost;
    bool            targetPostLiked;

    NavBar nav;

    std::unique_ptr<PostCard> postCard;

    sf::View commentViewport;
    float    scrollOffset;
    float    targetScrollOffset;
    float    maxScrollOffset;

    std::vector<std::unique_ptr<CommentCard>> commentCards;

    sf::RectangleShape           replyBackground;
    std::unique_ptr<TextInput>   replyInput;
    std::unique_ptr<GlassButton> replyButton;

public:
    PostDetailScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                     CommentManager& cm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;
    void update(float dt);

    void setTargetPost(const Post& post, bool liked);
    void setCurrentUser(User* user);
    void reloadComments();

    bool isHomeClicked(sf::Event& event);
    bool isSearchClicked(sf::Event& event);
    bool isProfileClicked(sf::Event& event);
    bool isLogoutClicked(sf::Event& event);
};

#endif // POSTDETAILSCREEN_H
