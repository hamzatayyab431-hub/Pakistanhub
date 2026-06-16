#include "PostCard.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <ctime>

static std::string relativeTime(time_t t) {
    time_t now = time(nullptr);
    long d = static_cast<long>(now - t);
    if (d < 0) d = 0;
    if (d < 60)     return "Just now";
    if (d < 3600)   return std::to_string(d/60)   + "m";
    if (d < 86400)  return std::to_string(d/3600) + "h";
    if (d < 604800) return std::to_string(d/86400) + "d";
    char buf[16]; strftime(buf,sizeof(buf),"%d %b",localtime(&t));
    return buf;
}

static std::string wrapText(const std::string& str, float width,
                              const sf::Font& font, unsigned int sz) {
    std::string result;
    sf::Text h; h.setFont(font); h.setCharacterSize(sz);
    std::stringstream ss(str);
    std::string para;
    while (std::getline(ss, para, '\n')) {
        std::stringstream ws(para);
        std::string word, line, out;
        while (ws >> word) {
            std::string test = line + (line.empty()?"":"  ") + word;
            h.setString(test);
            if (h.getGlobalBounds().width > width) {
                out += (out.empty()?"":"\n") + line; line = word;
            } else line = test;
        }
        out += (out.empty()?"":"\n") + line;
        result += (result.empty()?"":"\n") + out;
    }
    return result;
}

PostCard::PostCard(const Post& pst, sf::Font& fnt,
                   const sf::Vector2f& pos, const sf::Vector2f& sz,
                   bool liked, int commentsCount)
    : post(pst), font(fnt), position(pos), size(sz),
      isLiked(liked), isHovered(false), commentsCount(commentsCount)
{
    std::string wrapped = wrapText(post.getContent(), sz.x - 80.f, font, 15);
    int lines = static_cast<int>(std::count(wrapped.begin(),wrapped.end(),'\n')) + 1;
    size.y = 80.f + lines * 22.f + 36.f;   // header + content + actions
    if (size.y < 100.f) size.y = 100.f;

    authorText.setFont(font); authorText.setCharacterSize(15);
    authorText.setStyle(sf::Text::Bold); authorText.setFillColor(Theme::TEXT_WHITE);
    authorText.setString(post.getAuthorUsername());

    handleText.setFont(font); handleText.setCharacterSize(13);
    handleText.setFillColor(Theme::TEXT_DIM);
    handleText.setString("@" + post.getAuthorUsername());

    contentText.setFont(font); contentText.setCharacterSize(15);
    contentText.setFillColor(Theme::TEXT_WHITE); contentText.setString(wrapped);

    dateText.setFont(font); dateText.setCharacterSize(12);
    dateText.setFillColor(Theme::TEXT_DIM);
    dateText.setString(relativeTime(post.getTimestamp()));

    likeText.setFont(font);  likeText.setCharacterSize(13);
    commentText.setFont(font); commentText.setCharacterSize(13);
    commentText.setFillColor(Theme::TEXT_DIM);
    commentText.setString("Comment (" + std::to_string(commentsCount) + ")");

    hoverFillAlpha.speed = 12.f; hoverFillAlpha.snap(0.f);
    hoverGlowAlpha.speed = 12.f; hoverGlowAlpha.snap(0.f);
    likeColorAnim.speed  = 12.f;
    likeColorAnim.snap(isLiked ? Theme::GREEN : Theme::TEXT_DIM);

    setPosition(position);
    refreshLikeLabel();
}

void PostCard::setPosition(const sf::Vector2f& pos) {
    position = pos;
    backgroundRect.setPosition(pos);
    backgroundRect.setSize(size);

    accentBar.setPosition(pos.x, pos.y);   // kept but hidden (zero-width)
    accentBar.setSize({0.f, size.y});
    accentGlow.setPosition(pos.x, pos.y);
    accentGlow.setSize({0.f, size.y});

    float avatarCx = pos.x + 20.f + 19.f;
    float avatarCy = pos.y + 16.f + 19.f;
    avatarCenter = {avatarCx, avatarCy};

    float tx = pos.x + 60.f;
    authorText.setPosition(tx, pos.y + 14.f);
    float aw = authorText.getGlobalBounds().width;
    handleText.setPosition(tx + aw + 6.f, pos.y + 16.f);

    sf::FloatRect db = dateText.getLocalBounds();
    dateText.setPosition(pos.x + size.x - db.width - 16.f, pos.y + 16.f);

    contentText.setPosition(tx, pos.y + 44.f);

    // Buttons — Twitter/X style: row below content
    float btnY = pos.y + size.y - 30.f;
    float btnX = tx;

    likeButtonRect.setSize({110.f, 24.f});
    likeButtonRect.setPosition(btnX, btnY);
    likeButtonRect.setFillColor(sf::Color::Transparent);
    likeButtonRect.setOutlineThickness(0.f);
    likeText.setPosition(btnX + 2.f, btnY + 4.f);

    commentButtonRect.setSize({130.f, 24.f});
    commentButtonRect.setPosition(btnX + 124.f, btnY);
    commentButtonRect.setFillColor(sf::Color::Transparent);
    commentButtonRect.setOutlineThickness(0.f);
    commentText.setPosition(btnX + 126.f, btnY + 4.f);

    refreshLikeLabel();
}

void PostCard::refreshLikeLabel() {
    likeText.setString((isLiked ? "♥ " : "♡ ") +
        std::to_string(post.getLikeCount()));
}

void PostCard::toggleLikeState() {
    likeColorAnim.setTarget(isLiked ? Theme::ERROR : Theme::TEXT_DIM);
    refreshLikeLabel();
}

void PostCard::update(float dt) {
    hoverFillAlpha.update(dt);
    hoverGlowAlpha.update(dt);
    likeColorAnim.update(dt);
    likeText.setFillColor(likeColorAnim.get());
}

void PostCard::draw(sf::RenderWindow& window) {
    // Card bg — slightly lifted on hover
    sf::Uint8 fa = static_cast<sf::Uint8>(hoverFillAlpha.current);
    sf::Color fill = fa > 0 ? Theme::CARD_HOVER : Theme::CARD_BG;
    DrawUtils::drawRoundRect(window,
        sf::FloatRect(position.x, position.y, size.x, size.y),
        0.f, fill);

    // Bottom divider
    DrawUtils::drawDivider(window, position.x, position.y + size.y - 1.f, size.x);

    // Avatar
    if (!post.getAuthorUsername().empty())
        DrawUtils::drawAvatar(window, avatarCenter, 18.f,
            post.getAuthorUsername()[0], font, 14);

    window.draw(authorText);
    window.draw(handleText);
    window.draw(dateText);
    window.draw(contentText);

    window.draw(likeButtonRect);
    window.draw(likeText);
    window.draw(commentButtonRect);
    window.draw(commentText);
}

void PostCard::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        bool prev = isHovered;
        isHovered = backgroundRect.getGlobalBounds().contains(mp);
        if (isHovered != prev)
            hoverFillAlpha.setTarget(isHovered ? 255.f : 0.f);
    }
}

bool PostCard::isLikeClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        if (likeButtonRect.getGlobalBounds().contains(mp)) {
            isLiked = !isLiked;
            if (isLiked) post.setLikeCount(post.getLikeCount()+1);
            else          post.setLikeCount(std::max(0, post.getLikeCount()-1));
            toggleLikeState();
            return true;
        }
    }
    return false;
}

bool PostCard::isCommentClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        return commentButtonRect.getGlobalBounds().contains(mp);
    }
    return false;
}

bool PostCard::isHandleClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        return authorText.getGlobalBounds().contains(mp) ||
               handleText.getGlobalBounds().contains(mp);
    }
    return false;
}

int   PostCard::getPostId()  const { return post.getPostId(); }
float PostCard::getHeight()  const { return size.y; }
