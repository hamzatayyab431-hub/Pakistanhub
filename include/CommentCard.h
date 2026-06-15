#ifndef COMMENTCARD_H
#define COMMENTCARD_H

#include "UIComponent.h"
#include "Comment.h"

class CommentCard : public UIComponent {
private:
    Comment comment;
    sf::Font& font;
    sf::Vector2f position;
    sf::Vector2f size;

    sf::RectangleShape backgroundRect;
    sf::RectangleShape accentBar;
    sf::Text authorText;
    sf::Text dateText;
    sf::Text contentText;

public:
    CommentCard(const Comment& cmt, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;

    void setPosition(const sf::Vector2f& pos);
    float getHeight() const;
};

#endif // COMMENTCARD_H
