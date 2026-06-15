#include "CommentCard.h"
#include <sstream>
#include <algorithm>

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

CommentCard::CommentCard(const Comment& cmt, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz)
    : comment(cmt), font(fnt), position(pos), size(sz) {

    // Wrap comment text to the card's inner width
    std::string wrapped = wrapText(comment.getContent(), size.x - 40.0f, font, 14);

    // Compute dynamic height based on text line count
    size.y = 65.0f + (std::count(wrapped.begin(), wrapped.end(), '\n') + 1) * 20.0f;
    if (size.y < 80.0f) size.y = 80.0f;

    // Set positions and sizes
    setPosition(position);

    // Initial setup of text fields
    authorText.setFont(font);
    authorText.setCharacterSize(14);
    authorText.setStyle(sf::Text::Bold);
    authorText.setFillColor(sf::Color(0, 166, 81)); // Pakistan Green for comment author
    authorText.setString("@" + comment.getAuthorUsername());

    contentText.setFont(font);
    contentText.setCharacterSize(14);
    contentText.setFillColor(sf::Color::White);
    contentText.setString(wrapped);

    dateText.setFont(font);
    dateText.setCharacterSize(11);
    dateText.setFillColor(sf::Color(255, 255, 255, 120));
    dateText.setString(comment.getFormattedDate());

    // Update positions of text elements relative to position
    setPosition(position);
}

void CommentCard::setPosition(const sf::Vector2f& pos) {
    position = pos;

    // Background shapes
    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);
    backgroundRect.setFillColor(sf::Color(255, 255, 255, 30)); // 30 alpha white
    backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 60)); // 60 alpha white
    backgroundRect.setOutlineThickness(1.0f);

    // Green accent bar on left side
    accentBar.setPosition(position.x, position.y);
    accentBar.setSize(sf::Vector2f(3.0f, size.y));
    accentBar.setFillColor(sf::Color(0, 166, 81, 100)); // Green at alpha 100

    // Text positions (shifted right for accent bar)
    authorText.setPosition(position.x + 28.0f, position.y + 12.0f);
    
    // Position date dynamically after author text to prevent overlapping
    float authorWidth = authorText.getGlobalBounds().width;
    dateText.setPosition(position.x + 28.0f + authorWidth + 12.0f, position.y + 14.0f);

    contentText.setPosition(position.x + 28.0f, position.y + 38.0f);
}

void CommentCard::draw(sf::RenderWindow& window) {
    // Simple hover state check (on mouseover, slightly transition fill alpha)
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);
    if (backgroundRect.getGlobalBounds().contains(mousePos)) {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 50)); // Alpha 50 fill on hover
    } else {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 30)); // Default Alpha 30 fill
    }

    window.draw(backgroundRect);
    window.draw(accentBar);
    window.draw(authorText);
    window.draw(dateText);
    window.draw(contentText);
}

void CommentCard::handleEvent(sf::Event& event) {
    (void)event;
}

float CommentCard::getHeight() const {
    return size.y;
}
