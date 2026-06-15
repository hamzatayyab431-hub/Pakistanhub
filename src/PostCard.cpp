#include "PostCard.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <ctime>

// ─── helpers ─────────────────────────────────────────────────────────────────

static std::string relativeTime(time_t postTime) {
    time_t now = time(nullptr);
    long diff = static_cast<long>(now - postTime);
    if (diff < 0)   diff = 0;
    if (diff < 60)        return "Just now";
    if (diff < 3600)      return std::to_string(diff / 60)   + "m ago";
    if (diff < 86400)     return std::to_string(diff / 3600) + "h ago";
    if (diff < 604800)    return std::to_string(diff / 86400) + "d ago";
    char buf[20];
    strftime(buf, sizeof(buf), "%d %b", localtime(&postTime));
    return std::string(buf);
}

static std::string wrapText(const std::string& str,
                              float width,
                              const sf::Font& font,
                              unsigned int charSize)
{
    std::string result;
    sf::Text helper;
    helper.setFont(font);
    helper.setCharacterSize(charSize);

    std::stringstream ss(str);
    std::string paragraph;
    while (std::getline(ss, paragraph, '\n')) {
        std::stringstream words(paragraph);
        std::string word, line, para;
        while (words >> word) {
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

// ─── constructor ─────────────────────────────────────────────────────────────

PostCard::PostCard(const Post& pst, sf::Font& fnt,
                   const sf::Vector2f& pos, const sf::Vector2f& sz,
                   bool liked, int commentsCount)
    : post(pst), font(fnt), position(pos), size(sz),
      isLiked(liked), isHovered(false), commentsCount(commentsCount)
{
    std::string wrapped = wrapText(post.getContent(), sz.x - 75.f, font, 14);
    int lines = static_cast<int>(std::count(wrapped.begin(), wrapped.end(), '\n')) + 1;
    size.y = 95.f + lines * 22.f;
    if (size.y < 110.f) size.y = 110.f;

    // Author / handle
    authorText.setFont(font);
    authorText.setCharacterSize(15);
    authorText.setStyle(sf::Text::Bold);
    authorText.setFillColor(Theme::TEXT_WHITE);
    authorText.setString(post.getAuthorUsername());

    handleText.setFont(font);
    handleText.setCharacterSize(13);
    handleText.setFillColor(Theme::TEXT_MUTED);
    handleText.setString("@" + post.getAuthorUsername());

    contentText.setFont(font);
    contentText.setCharacterSize(14);
    contentText.setFillColor(Theme::TEXT_WHITE);
    contentText.setString(wrapped);

    dateText.setFont(font);
    dateText.setCharacterSize(12);
    dateText.setFillColor(Theme::TEXT_DIM);
    dateText.setString(relativeTime(post.getTimestamp()));

    likeText.setFont(font);
    likeText.setCharacterSize(13);

    commentText.setFont(font);
    commentText.setCharacterSize(13);
    commentText.setFillColor(Theme::TEXT_MUTED);
    commentText.setString("Comment (" + std::to_string(commentsCount) + ")");

    // Animations
    hoverFillAlpha.speed = 10.f;  hoverFillAlpha.snap(0.f);
    hoverGlowAlpha.speed = 10.f;  hoverGlowAlpha.snap(0.f);
    likeColorAnim.speed  = 10.f;
    likeColorAnim.snap(isLiked ? Theme::GREEN : Theme::TEXT_MUTED);

    setPosition(position);
    refreshLikeLabel();
}

// ─── setPosition ─────────────────────────────────────────────────────────────

void PostCard::setPosition(const sf::Vector2f& pos) {
    position = pos;
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);

    accentBar.setPosition(position.x, position.y);
    accentBar.setSize(sf::Vector2f(3.f, size.y));
    accentBar.setFillColor(Theme::GREEN);

    accentGlow.setPosition(position.x, position.y);
    accentGlow.setSize(sf::Vector2f(12.f, size.y));

    float avatarCx = position.x + 8.f + 19.f;   // 8px left pad + radius
    float avatarCy = position.y + 14.f + 19.f;
    avatarCenter = sf::Vector2f(avatarCx, avatarCy);

    // Text layout (starts after avatar: x+52)
    float tx = position.x + 52.f;
    authorText.setPosition(tx, position.y + 14.f);
    float aw = authorText.getGlobalBounds().width;
    handleText.setPosition(tx + aw + 8.f, position.y + 16.f);

    // Date: right-aligned
    sf::FloatRect db = dateText.getLocalBounds();
    dateText.setPosition(position.x + size.x - db.width - 12.f, position.y + 16.f);

    contentText.setPosition(tx, position.y + 42.f);

    // Action buttons — placed left-anchored so they never overlap
    float btnY = position.y + size.y - 34.f;
    float btnX = position.x + 52.f;   // align with text column

    likeButtonRect.setSize(sf::Vector2f(120.f, 26.f));
    likeButtonRect.setPosition(btnX, btnY);
    likeButtonRect.setFillColor(Theme::GLASS_1);
    likeButtonRect.setOutlineThickness(1.f);

    likeText.setPosition(btnX + 10.f, btnY + 5.f);

    commentButtonRect.setSize(sf::Vector2f(140.f, 26.f));
    commentButtonRect.setPosition(btnX + 132.f, btnY);   // 12px gap after like button
    commentButtonRect.setFillColor(Theme::GLASS_1);
    commentButtonRect.setOutlineColor(Theme::GLASS_BORDER);
    commentButtonRect.setOutlineThickness(1.f);

    commentText.setPosition(btnX + 142.f, btnY + 5.f);

    refreshLikeLabel();
}

// ─── refreshLikeLabel ────────────────────────────────────────────────────────

void PostCard::refreshLikeLabel() {
    if (isLiked) {
        likeText.setString("Liked (" + std::to_string(post.getLikeCount()) + ")");
        likeButtonRect.setOutlineColor(Theme::GREEN);
    } else {
        likeText.setString("Like  (" + std::to_string(post.getLikeCount()) + ")");
        likeButtonRect.setOutlineColor(Theme::GLASS_BORDER);
    }
}

// ─── toggleLikeState ─────────────────────────────────────────────────────────

void PostCard::toggleLikeState() {
    likeColorAnim.setTarget(isLiked ? Theme::GREEN : Theme::TEXT_MUTED);
    refreshLikeLabel();
}

// ─── update ──────────────────────────────────────────────────────────────────

void PostCard::update(float dt) {
    hoverFillAlpha.update(dt);
    hoverGlowAlpha.update(dt);
    likeColorAnim.update(dt);

    // Apply like colour
    likeText.setFillColor(likeColorAnim.get());

    // Apply accent glow
    sf::Color gc = Theme::GREEN_GLOW;
    gc.a = static_cast<sf::Uint8>(hoverGlowAlpha.current);
    accentGlow.setFillColor(gc);
}

// ─── draw ────────────────────────────────────────────────────────────────────

void PostCard::draw(sf::RenderWindow& window) {
    // Panel fill — gets brighter on hover
    sf::Uint8 fillA = static_cast<sf::Uint8>(12 + hoverFillAlpha.current);
    sf::Color panelFill(255, 255, 255, fillA);
    DrawUtils::drawGlassPanel(window,
        sf::FloatRect(position.x, position.y, size.x, size.y),
        6.f, panelFill, false);

    // Accent glow (behind accent bar)
    window.draw(accentGlow);

    // Accent bar
    window.draw(accentBar);

    // Avatar (38px diameter = radius 19)
    if (!post.getAuthorUsername().empty()) {
        DrawUtils::drawAvatar(window, avatarCenter, 19.f,
            post.getAuthorUsername()[0], font, 16);
    }

    window.draw(authorText);
    window.draw(handleText);
    window.draw(dateText);
    window.draw(contentText);

    // Divider
    sf::RectangleShape div(sf::Vector2f(size.x - 16.f, 1.f));
    div.setPosition(position.x + 8.f, position.y + size.y - 38.f);
    div.setFillColor(Theme::GLASS_BORDER);
    window.draw(div);

    window.draw(likeButtonRect);
    window.draw(likeText);
    window.draw(commentButtonRect);
    window.draw(commentText);
}

// ─── handleEvent ─────────────────────────────────────────────────────────────

void PostCard::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        bool prev = isHovered;
        isHovered = backgroundRect.getGlobalBounds().contains(mp);
        if (isHovered != prev) {
            hoverFillAlpha.setTarget(isHovered ? 16.f : 0.f);
            hoverGlowAlpha.setTarget(isHovered ? 40.f : 0.f);
        }

        // Comment button hover
        if (commentButtonRect.getGlobalBounds().contains(mp)) {
            commentButtonRect.setFillColor(Theme::GLASS_2);
            commentButtonRect.setOutlineColor(Theme::GREEN_DIM);
        } else {
            commentButtonRect.setFillColor(Theme::GLASS_1);
            commentButtonRect.setOutlineColor(Theme::GLASS_BORDER);
        }

        // Like button hover
        if (likeButtonRect.getGlobalBounds().contains(mp)) {
            likeButtonRect.setFillColor(Theme::GLASS_2);
        } else {
            likeButtonRect.setFillColor(Theme::GLASS_1);
        }
    }
}

// ─── interaction queries ─────────────────────────────────────────────────────

bool PostCard::isLikeClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        if (likeButtonRect.getGlobalBounds().contains(mp)) {
            isLiked = !isLiked;
            if (isLiked) post.setLikeCount(post.getLikeCount() + 1);
            else          post.setLikeCount(std::max(0, post.getLikeCount() - 1));
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

int   PostCard::getPostId()   const { return post.getPostId(); }
float PostCard::getHeight()   const { return size.y; }
