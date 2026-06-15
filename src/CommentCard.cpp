#include "CommentCard.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <sstream>
#include <algorithm>
#include <ctime>

static std::string relativeTime(time_t postTime) {
    time_t now = time(nullptr);
    long diff = static_cast<long>(now - postTime);
    if (diff < 0)   diff = 0;
    if (diff < 60)        return "Just now";
    if (diff < 3600)      return std::to_string(diff / 60)    + "m ago";
    if (diff < 86400)     return std::to_string(diff / 3600)  + "h ago";
    if (diff < 604800)    return std::to_string(diff / 86400) + "d ago";
    char buf[20];
    strftime(buf, sizeof(buf), "%d %b", localtime(&postTime));
    return std::string(buf);
}

static std::string wrapText(const std::string& str, float width,
                              const sf::Font& font, unsigned int charSize) {
    std::string result;
    sf::Text helper;
    helper.setFont(font);
    helper.setCharacterSize(charSize);
    std::stringstream ss(str);
    std::string paragraph;
    while (std::getline(ss, paragraph, '\n')) {
        std::stringstream ws(paragraph);
        std::string word, line, para;
        while (ws >> word) {
            std::string test = line + (line.empty() ? "" : " ") + word;
            helper.setString(test);
            if (helper.getGlobalBounds().width > width) {
                para += (para.empty() ? "" : "\n") + line;
                line = word;
            } else { line = test; }
        }
        para += (para.empty() ? "" : "\n") + line;
        result += (result.empty() ? "" : "\n") + para;
    }
    return result;
}

CommentCard::CommentCard(const Comment& cmt, sf::Font& fnt,
                         const sf::Vector2f& pos, const sf::Vector2f& sz)
    : comment(cmt), font(fnt), position(pos), size(sz)
{
    std::string wrapped = wrapText(comment.getContent(), sz.x - 55.f, font, 13);
    int lines = static_cast<int>(std::count(wrapped.begin(), wrapped.end(), '\n')) + 1;
    size.y = 55.f + lines * 20.f;
    if (size.y < 70.f) size.y = 70.f;

    authorText.setFont(font);
    authorText.setCharacterSize(13);
    authorText.setStyle(sf::Text::Bold);
    authorText.setFillColor(Theme::GREEN);

    dateText.setFont(font);
    dateText.setCharacterSize(12);
    dateText.setFillColor(Theme::TEXT_DIM);
    dateText.setString(relativeTime(comment.getTimestamp()));

    contentText.setFont(font);
    contentText.setCharacterSize(13);
    contentText.setFillColor(Theme::TEXT_WHITE);
    contentText.setString(wrapped);

    setPosition(position);
    authorText.setString("@" + comment.getAuthorUsername());
}

void CommentCard::setPosition(const sf::Vector2f& pos) {
    position = pos;
    backgroundRect.setPosition(pos);
    backgroundRect.setSize(size);

    accentBar.setPosition(pos.x, pos.y);
    accentBar.setSize(sf::Vector2f(3.f, size.y));
    accentBar.setFillColor(Theme::GREEN);

    float tx = pos.x + 20.f;
    authorText.setPosition(tx, pos.y + 10.f);
    float aw = authorText.getGlobalBounds().width;
    dateText.setPosition(tx + aw + 10.f, pos.y + 12.f);
    contentText.setPosition(tx, pos.y + 34.f);
}

void CommentCard::draw(sf::RenderWindow& window) {
    DrawUtils::drawGlassPanel(window,
        sf::FloatRect(position.x, position.y, size.x, size.y),
        4.f, Theme::GLASS_1);

    window.draw(accentBar);
    window.draw(authorText);
    window.draw(dateText);
    window.draw(contentText);
}

void CommentCard::handleEvent(sf::Event&) {}

float CommentCard::getHeight() const { return size.y; }
