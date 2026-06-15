#include "PostCard.h"
#include <sstream>
#include <algorithm>

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

PostCard::PostCard(const Post& pst, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz, bool liked)
    : post(pst), font(fnt), position(pos), size(sz), isLiked(liked), isHovered(false) {

    // Wrap post text to the card's inner width
    std::string wrapped = wrapText(post.getContent(), size.x - 40.0f, font, 15);

    // Compute dynamic height based on the text line count
    size.y = 90.0f + (std::count(wrapped.begin(), wrapped.end(), '\n') + 1) * 21.0f;
    if (size.y < 120.0f) size.y = 120.0f;

    // Set positions and sizes
    setPosition(position);

    // Initial setup of text fields
    authorText.setFont(font);
    authorText.setCharacterSize(16);
    authorText.setStyle(sf::Text::Bold);
    authorText.setFillColor(sf::Color::White);
    authorText.setString(post.getAuthorUsername());

    handleText.setFont(font);
    handleText.setCharacterSize(14);
    handleText.setFillColor(sf::Color(255, 255, 255, 140));
    handleText.setString("@" + post.getAuthorUsername());

    contentText.setFont(font);
    contentText.setCharacterSize(15);
    contentText.setFillColor(sf::Color::White);
    contentText.setString(wrapped);

    dateText.setFont(font);
    dateText.setCharacterSize(12);
    dateText.setFillColor(sf::Color(255, 255, 255, 130));
    dateText.setString(post.getFormattedDate());

    likeText.setFont(font);
    likeText.setCharacterSize(12);

    // Update positions of text elements relative to position
    setPosition(position);
}

void PostCard::setPosition(const sf::Vector2f& pos) {
    position = pos;

    // Background and shadow boxes
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);
    backgroundRect.setFillColor(sf::Color(255, 255, 255, 20));
    backgroundRect.setOutlineThickness(1.0f);

    shadowRect.setPosition(position - sf::Vector2f(2, 2));
    shadowRect.setSize(size + sf::Vector2f(4, 4));
    shadowRect.setFillColor(sf::Color(0, 166, 81, 35));

    // Text positions
    authorText.setPosition(position.x + 20.0f, position.y + 15.0f);
    
    // Position handle dynamically after author text to prevent overlapping
    float authorWidth = authorText.getGlobalBounds().width;
    handleText.setPosition(position.x + 20.0f + authorWidth + 8.0f, position.y + 17.0f);

    contentText.setPosition(position.x + 20.0f, position.y + 45.0f);
    dateText.setPosition(position.x + 20.0f, position.y + size.y - 30.0f);

    // Like button placement
    likeButtonRect.setSize(sf::Vector2f(100.0f, 25.0f));
    likeButtonRect.setPosition(position.x + size.x - 120.0f, position.y + size.y - 32.0f);
    likeButtonRect.setOutlineThickness(1.0f);

    likeText.setPosition(position.x + size.x - 110.0f, position.y + size.y - 28.0f);

    toggleLikeState(); // Refresh color/labels
}

void PostCard::toggleLikeState() {
    if (isLiked) {
        likeText.setFillColor(sf::Color(0, 166, 81)); // Green liked color
        likeText.setString("Liked (" + std::to_string(post.getLikeCount()) + ")");
        likeButtonRect.setOutlineColor(sf::Color(0, 166, 81, 200));
        likeButtonRect.setFillColor(sf::Color(0, 166, 81, 20));
    } else {
        likeText.setFillColor(sf::Color(255, 255, 255, 150));
        likeText.setString("Like (" + std::to_string(post.getLikeCount()) + ")");
        likeButtonRect.setOutlineColor(sf::Color(255, 255, 255, 50));
        likeButtonRect.setFillColor(sf::Color(255, 255, 255, 10));
    }
}

void PostCard::draw(sf::RenderWindow& window) {
    if (isHovered) {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 45)); // Muted hover glow
        backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 120));
        window.draw(shadowRect);
    } else {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 20));
        backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 80));
    }

    window.draw(backgroundRect);
    window.draw(authorText);
    window.draw(handleText);
    window.draw(contentText);
    window.draw(dateText);
    window.draw(likeButtonRect);
    window.draw(likeText);
}

void PostCard::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isHovered = backgroundRect.getGlobalBounds().contains(mousePos);
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

int PostCard::getPostId() const {
    return post.getPostId();
}

float PostCard::getHeight() const {
    return size.y;
}
