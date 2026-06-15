#include "PostCard.h"
#include "GlassPanel.h"
#include "Theme.h"
#include <sstream>
#include <algorithm>
#include <cctype>

// Local helper to wrap text to a maximum width in pixels
static std::string wrapText(const std::string& str, float width, const sf::Font& font, unsigned int charSize) {
    std::string result = "";
    sf::Text textHelper;
    textHelper.setFont(font);
    textHelper.setCharacterSize(charSize);

    std::stringstream ss(str);
    std::string paragraph;
    while (std::getline(ss, paragraph, '\n')) {
        std::stringstream wordStream(paragraph);
        std::string word;
        std::string currentLine = "";
        std::string paragraphResult = "";

        while (wordStream >> word) {
            std::string testLine = currentLine + (currentLine.empty() ? "" : " ") + word;
            textHelper.setString(testLine);
            if (textHelper.getGlobalBounds().width > width) {
                paragraphResult += (paragraphResult.empty() ? "" : "\n") + currentLine;
                currentLine = word;
            } else {
                currentLine = testLine;
            }
        }
        if (!currentLine.empty()) {
            paragraphResult += (paragraphResult.empty() ? "" : "\n") + currentLine;
        }
        result += (result.empty() ? "" : "\n") + paragraphResult;
    }
    return result;
}

PostCard::PostCard(const Post& pst, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz, bool liked, int commentsCount)
    : post(pst), font(fnt), position(pos), size(sz), isLiked(liked), isHovered(false) {

    // Wrap post text to the card's inner width
    std::string wrapped = wrapText(post.getContent(), size.x - 40.0f, font, 15);

    // Compute dynamic height based on the text line count
    size.y = 90.0f + (std::count(wrapped.begin(), wrapped.end(), '\n') + 1) * 21.0f;
    if (size.y < 120.0f) size.y = 120.0f;

    // Initial setup of text fields
    authorText.setFont(font);
    authorText.setCharacterSize(16);
    authorText.setStyle(sf::Text::Bold);
    authorText.setFillColor(sf::Color::White);
    authorText.setString(post.getAuthorUsername());

    handleText.setFont(font);
    handleText.setCharacterSize(14);
    handleText.setFillColor(Theme::TEXT_MUTED);
    handleText.setString("@" + post.getAuthorUsername());

    contentText.setFont(font);
    contentText.setCharacterSize(15);
    contentText.setFillColor(Theme::TEXT_PRIMARY);
    contentText.setString(wrapped);

    dateText.setFont(font);
    dateText.setCharacterSize(12);
    dateText.setFillColor(Theme::TEXT_MUTED);
    dateText.setString(post.getFormattedDate());

    likeText.setFont(font);
    likeText.setCharacterSize(12);

    commentText.setFont(font);
    commentText.setCharacterSize(12);
    commentText.setFillColor(Theme::TEXT_MUTED);
    commentText.setString("Comments (" + std::to_string(commentsCount) + ")");

    // Avatar circle
    avatarCircle.setRadius(20);
    avatarCircle.setOutlineColor(Theme::GREEN_PRIMARY);
    avatarCircle.setOutlineThickness(1.5f);
    avatarCircle.setFillColor(sf::Color::Transparent);
    avatarCircle.setPosition(position.x + 20, position.y + 15);

    // Avatar letter (first char of author username, uppercased)
    avatarLetter.setFont(font);
    avatarLetter.setCharacterSize(18);
    avatarLetter.setStyle(sf::Text::Bold);
    avatarLetter.setFillColor(sf::Color::White);
    std::string authorName = post.getAuthorUsername();
    if (!authorName.empty()) {
        avatarLetter.setString(std::string(1, static_cast<char>(std::toupper(authorName[0]))));
    }

    // Accent bar
    accentBar.setSize(sf::Vector2f(4.0f, size.y));
    accentBar.setFillColor(sf::Color(0, 166, 81));
    accentBar.setPosition(position.x, position.y);

    // Set positions and sizes
    setPosition(position);
}

void PostCard::setPosition(const sf::Vector2f& pos) {
    position = pos;

    // Background rect used for bounds logic only
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);

    // Accent bar
    accentBar.setPosition(position.x, position.y);
    accentBar.setSize(sf::Vector2f(4.0f, size.y));

    // Avatar circle
    avatarCircle.setPosition(position.x + 16.0f, position.y + 14.0f);
    avatarCircle.setRadius(20);

    // Center avatar letter inside the circle
    {
        sf::FloatRect letterBounds = avatarLetter.getLocalBounds();
        float cx = position.x + 16.0f + 20.0f; // circle center x
        float cy = position.y + 14.0f + 20.0f; // circle center y
        avatarLetter.setOrigin(letterBounds.left + letterBounds.width / 2.0f, letterBounds.top + letterBounds.height / 2.0f);
        avatarLetter.setPosition(cx, cy);
    }

    // Text positions (shifted right by 36px for avatar)
    authorText.setPosition(position.x + 56.0f, position.y + 15.0f);
    
    // Position handle dynamically after author text to prevent overlapping
    float authorWidth = authorText.getGlobalBounds().width;
    handleText.setPosition(position.x + 56.0f + authorWidth + 8.0f, position.y + 17.0f);

    contentText.setPosition(position.x + 56.0f, position.y + 45.0f);
    dateText.setPosition(position.x + 56.0f, position.y + size.y - 30.0f);

    // Like button placement
    likeButtonRect.setSize(sf::Vector2f(100.0f, 25.0f));
    likeButtonRect.setPosition(position.x + size.x - 230.0f, position.y + size.y - 32.0f);
    likeButtonRect.setOutlineThickness(1.0f);

    likeText.setPosition(position.x + size.x - 220.0f, position.y + size.y - 28.0f);

    // Comment button placement
    commentButtonRect.setSize(sf::Vector2f(140.0f, 25.0f));
    commentButtonRect.setPosition(position.x + size.x - 150.0f, position.y + size.y - 32.0f);
    commentButtonRect.setOutlineThickness(1.0f);
    commentButtonRect.setFillColor(sf::Color(255, 255, 255, 30));
    commentButtonRect.setOutlineColor(sf::Color(255, 255, 255, 60));

    commentText.setPosition(position.x + size.x - 140.0f, position.y + size.y - 28.0f);

    toggleLikeState(); // Refresh color/labels
}

void PostCard::toggleLikeState() {
    if (isLiked) {
        likeText.setFillColor(Theme::GREEN_PRIMARY); // Green liked color
        likeText.setString("Liked (" + std::to_string(post.getLikeCount()) + ")");
        likeButtonRect.setOutlineColor(Theme::GREEN_PRIMARY);
        likeButtonRect.setFillColor(Theme::GLASS_FILL);
    } else {
        likeText.setFillColor(Theme::TEXT_MUTED);
        likeText.setString("Like (" + std::to_string(post.getLikeCount()) + ")");
        likeButtonRect.setOutlineColor(Theme::GLASS_BORDER);
        likeButtonRect.setFillColor(Theme::GLASS_FILL);
    }
}

void PostCard::draw(sf::RenderWindow& window) {
    GlassPanel::draw(window, backgroundRect.getGlobalBounds(), isHovered, 1.0f);
    window.draw(accentBar);
    window.draw(avatarCircle);
    window.draw(avatarLetter);
    window.draw(authorText);
    window.draw(handleText);
    window.draw(contentText);
    window.draw(dateText);
    window.draw(likeButtonRect);
    window.draw(likeText);
    window.draw(commentButtonRect);
    window.draw(commentText);
}

void PostCard::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isHovered = backgroundRect.getGlobalBounds().contains(mousePos);

        // Highlight comment button on hover
        if (commentButtonRect.getGlobalBounds().contains(mousePos)) {
            commentButtonRect.setFillColor(sf::Color(255, 255, 255, 50));
            commentButtonRect.setOutlineColor(sf::Color(0, 166, 81, 200));
        } else {
            commentButtonRect.setFillColor(sf::Color(255, 255, 255, 30));
            commentButtonRect.setOutlineColor(sf::Color(255, 255, 255, 60));
        }
    }
}

bool PostCard::isLikeClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (likeButtonRect.getGlobalBounds().contains(mousePos)) {
            isLiked = !isLiked;
            if (isLiked) {
                post.setLikeCount(post.getLikeCount() + 1);
            } else {
                post.setLikeCount(std::max(0, post.getLikeCount() - 1));
            }
            toggleLikeState();
            return true;
        }
    }
    return false;
}

bool PostCard::isCommentClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return commentButtonRect.getGlobalBounds().contains(mousePos);
    }
    return false;
}

bool PostCard::isHandleClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return authorText.getGlobalBounds().contains(mousePos) || handleText.getGlobalBounds().contains(mousePos);
    }
    return false;
}

int PostCard::getPostId() const {
    return post.getPostId();
}

float PostCard::getHeight() const {
    return size.y;
}
