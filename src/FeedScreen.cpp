#include "FeedScreen.h"
#include <algorithm>
#include <iostream>

// Local helper to center sf::Text horizontally
static void centerTextHorizontally(sf::Text& text, float centerX) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.0f);
    text.setPosition(centerX, text.getPosition().y);
}

FeedScreen::FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm)
    : font(fnt), userManager(um), postManager(pm), currentUser(nullptr), scrollOffset(0.0f), maxScrollOffset(0.0f) {

    // Configure header top bar
    headerBackground.setPosition(0.0f, 0.0f);
    headerBackground.setSize(sf::Vector2f(1280.0f, 60.0f));
    headerBackground.setFillColor(sf::Color(13, 17, 23, 220)); // Deep dark near-black
    headerBackground.setOutlineColor(sf::Color(255, 255, 255, 40));
    headerBackground.setOutlineThickness(1.0f);

    // App logo
    logoText.setFont(font);
    logoText.setCharacterSize(22);
    logoText.setStyle(sf::Text::Bold);
    logoText.setFillColor(sf::Color(0, 166, 81)); // Pakistan Green #00A651
    logoText.setString("PakistanHub");
    logoText.setPosition(40.0f, 15.0f);

    // Profile text (will set string dynamically)
    profileText.setFont(font);
    profileText.setCharacterSize(14);
    profileText.setFillColor(sf::Color::White);
    profileText.setPosition(920.0f, 20.0f);

    // Logout text button
    logoutText.setFont(font);
    logoutText.setCharacterSize(14);
    logoutText.setFillColor(sf::Color(220, 53, 69)); // Red button
    logoutText.setString("Logout");
    logoutText.setPosition(1180.0f, 20.0f);

    // Compose panel background
    composePanel.setPosition(100.0f, 80.0f);
    composePanel.setSize(sf::Vector2f(1080.0f, 110.0f));
    composePanel.setFillColor(sf::Color(255, 255, 255, 15)); // Frosted panel
    composePanel.setOutlineColor(sf::Color(255, 255, 255, 50));
    composePanel.setOutlineThickness(1.0f);

    // Compose input field
    composeInput = std::make_unique<TextInput>(
        sf::Vector2f(120.0f, 95.0f),
        sf::Vector2f(800.0f, 80.0f),
        font, "What's on your mind?..."
    );

    // Compose Post button
    postButton = std::make_unique<GlassButton>(
        sf::Vector2f(940.0f, 95.0f),
        sf::Vector2f(220.0f, 80.0f),
        font, "POST"
    );

    // Setup scrollable view (clips inside y = 210 to y = 700)
    feedViewport.setSize(1080.0f, 490.0f);
    feedViewport.setViewport(sf::FloatRect(100.0f / 1280.0f, 210.0f / 720.0f, 1080.0f / 1280.0f, 490.0f / 720.0f));
    feedViewport.setCenter(540.0f, 245.0f);
}

void FeedScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser != nullptr) {
        profileText.setString(currentUser->getDisplayName() + " (@" + currentUser->getUsername() + ")");
        
        // Reposition logout text slightly depending on name length
        sf::FloatRect pBounds = profileText.getGlobalBounds();
        logoutText.setPosition(std::max(1180.0f, pBounds.left + pBounds.width + 30.0f), 20.0f);
    }
}

void FeedScreen::reloadFeed() {
    postCards.clear();
    scrollOffset = 0.0f;
    feedViewport.setCenter(540.0f, 245.0f);

    // Load sorted posts
    std::vector<Post> sortedPosts = postManager.getAllPostsSorted();
    
    float y = 10.0f;
    for (const auto& post : sortedPosts) {
        // Instantiate card at (0, y) relative to feedViewport
        auto card = std::make_unique<PostCard>(post, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 120.0f), false);
        y += card->getHeight() + 15.0f; // Add height and card spacing
        postCards.push_back(std::move(card));
    }

    maxScrollOffset = std::max(0.0f, y - 490.0f);
}

void FeedScreen::draw(sf::RenderWindow& window) {
    // 1. Draw static Header
    window.draw(headerBackground);
    window.draw(logoText);
    window.draw(profileText);
    window.draw(logoutText);

    // 2. Draw Compose area
    window.draw(composePanel);
    composeInput->draw(window);
    postButton->draw(window);

    // 3. Draw scrollable feed area with clipping view
    sf::View originalView = window.getView();
    
    // Apply viewport view
    window.setView(feedViewport);
    for (const auto& card : postCards) {
        card->draw(window);
    }

    // Restore original view
    window.setView(originalView);
}

void FeedScreen::handleEvent(sf::Event& event) {
    // Pass events to compose input and button
    composeInput->handleEvent(event);
    postButton->handleEvent(event);

    // Handle mouse move highlight for logout button
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (logoutText.getGlobalBounds().contains(mousePos)) {
            logoutText.setFillColor(sf::Color(220, 53, 69, 255)); // Bright red
        } else {
            logoutText.setFillColor(sf::Color(220, 53, 69, 180));
        }
    }

    if (currentUser != nullptr && postButton->isClicked(event)) {
        std::string content = composeInput->getText();
        if (!content.empty()) {
            postManager.createPost(currentUser->getUsername(), content);
            composeInput->clear();
            reloadFeed();
        }
    }

    // Scroll wheel offset adjustment
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        scrollOffset -= event.mouseWheelScroll.delta * 40.0f; // Scroll speed
        if (scrollOffset < 0.0f) scrollOffset = 0.0f;
        if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;
        
        feedViewport.setCenter(540.0f, 245.0f + scrollOffset);
    }

    // Map mouse events from screen coords to feedViewport coordinates for scrollable cards
    sf::Event mappedEvent = event;
    bool hasMouse = false;
    
    // We need window reference to map coordinates. We will get window context.
    // Wait, the app calls feedScreen->handleEvent(event).
    // Can we map coordinates without window? We need window to map pixel to coords.
    // But since the viewport has a fixed screen rectangle, we can map manually!
    // feedViewport screen rect is: X: [100, 1180], Y: [210, 700].
    // Mapped X = (screenX - 100) * (1080 / 1080) = screenX - 100.
    // Mapped Y = (screenY - 210) + scrollOffset.
    // This is incredibly simple and doesn't require window mapping!
    
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        float sx = static_cast<float>(event.mouseButton.x);
        float sy = static_cast<float>(event.mouseButton.y);
        mappedEvent.mouseButton.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseButton.y = static_cast<int>((sy - 210.0f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        float sx = static_cast<float>(event.mouseMove.x);
        float sy = static_cast<float>(event.mouseMove.y);
        mappedEvent.mouseMove.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseMove.y = static_cast<int>((sy - 210.0f) + scrollOffset);
        hasMouse = true;
    }

    // Pass mapped events to scrollable post cards
    for (auto& card : postCards) {
        card->handleEvent(mappedEvent);
        
        if (hasMouse && card->isLikeClicked(mappedEvent)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
        }
    }
}

bool FeedScreen::isLogoutClicked(sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return logoutText.getGlobalBounds().contains(mousePos);
    }
    return false;
}
