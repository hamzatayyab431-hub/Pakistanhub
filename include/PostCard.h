#ifndef POSTCARD_H
#define POSTCARD_H

#include "UIComponent.h"
#include "Post.h"
#include "Animator.h"

class PostCard : public UIComponent {
private:
    Post         post;
    sf::Font&    font;
    sf::Vector2f position;
    sf::Vector2f size;
    bool         isLiked;
    bool         isHovered;
    int          commentsCount;

    sf::RectangleShape backgroundRect;
    sf::RectangleShape accentBar;
    sf::RectangleShape accentGlow;
    sf::Vector2f       avatarCenter;

    sf::Text authorText;
    sf::Text handleText;
    sf::Text dateText;
    sf::Text contentText;
    sf::Text likeText;
    sf::Text commentText;

    sf::RectangleShape likeButtonRect;
    sf::RectangleShape commentButtonRect;

    // Animations
    Lerp      hoverFillAlpha;
    Lerp      hoverGlowAlpha;
    LerpColor likeColorAnim;

    void refreshLikeLabel();

public:
    PostCard(const Post& pst, sf::Font& fnt,
             const sf::Vector2f& pos, const sf::Vector2f& sz,
             bool liked = false, int commentsCount = 0);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update(float dt = 0.016f);

    bool isLikeClicked(sf::Event& event);
    bool isCommentClicked(sf::Event& event);
    bool isHandleClicked(sf::Event& event);

    std::string getAuthorUsername() const { return post.getAuthorUsername(); }
    int  getPostId()  const;
    bool getIsLiked() const { return isLiked; }
    void setPosition(const sf::Vector2f& pos);
    float getHeight() const;
    void toggleLikeState();
};

#endif // POSTCARD_H
