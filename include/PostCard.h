#ifndef POSTCARD_H
#define POSTCARD_H

#include "UIComponent.h"
#include "Post.h"

class PostCard : public UIComponent {
private:
    Post post;
    sf::Font& font;
    sf::Vector2f position;
    sf::Vector2f size;
    bool isLiked;
    bool isHovered;

    sf::RectangleShape backgroundRect;
    sf::RectangleShape shadowRect; // Green outline glow on hover
    sf::Text authorText;
    sf::Text handleText;
    sf::Text contentText;
    sf::Text dateText;

    // Like button shapes
    sf::RectangleShape likeButtonRect;
    sf::Text likeText;

public:
    PostCard(const Post& pst, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz, bool liked = false);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    bool isLikeClicked(sf::Event& event);
    bool isHandleClicked(sf::Event& event);
    std::string getAuthorUsername() const { return post.getAuthorUsername(); }
    int getPostId() const;
    bool getIsLiked() const { return isLiked; }
    void setPosition(const sf::Vector2f& pos);
    float getHeight() const;
    void toggleLikeState();
};

#endif // POSTCARD_H
