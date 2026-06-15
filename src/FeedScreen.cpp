#include "FeedScreen.h"
#include <algorithm>
#include <iostream>

// Local helper to center sf::Text horizontally
static void centerTextHorizontally(sf::Text& text, float centerX) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.0f);
    text.setPosition(centerX, text.getPosition().y);
}

FeedScreen::FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), socialGraph(sg), currentUser(nullptr),
      activeTab(0), scrollOffset(0.0f), maxScrollOffset(0.0f), clickedHandle("") {

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

    // Profile text
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

    // Tabs UI Setup
    forYouText.setFont(font);
    forYouText.setCharacterSize(16);
    forYouText.setStyle(sf::Text::Bold);
    forYouText.setString("For You");
    forYouText.setPosition(120.0f, 205.0f);

    followingText.setFont(font);
    followingText.setCharacterSize(16);
    followingText.setStyle(sf::Text::Bold);
    followingText.setString("Following");
    followingText.setPosition(220.0f, 205.0f);

    tabIndicator.setSize(sf::Vector2f(70.0f, 3.0f));
    tabIndicator.setFillColor(sf::Color(0, 166, 81));
    tabIndicator.setPosition(120.0f, 230.0f);

    // Setup scrollable view (clips inside y = 240 to y = 710)
    feedViewport.setSize(1080.0f, 460.0f);
    feedViewport.setViewport(sf::FloatRect(100.0f / 1280.0f, 240.0f / 720.0f, 1080.0f / 1280.0f, 460.0f / 720.0f));
    feedViewport.setCenter(540.0f, 230.0f);
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
    feedViewport.setCenter(540.0f, 230.0f);

    // Load sorted posts
    std::vector<Post> sortedPosts = postManager.getAllPostsSorted();
    
    // Position tab indicator underline
    if (activeTab == 0) {
        forYouText.setFillColor(sf::Color(0, 166, 81));
        followingText.setFillColor(sf::Color(255, 255, 255, 150));
        tabIndicator.setSize(sf::Vector2f(forYouText.getGlobalBounds().width, 3.0f));
        tabIndicator.setPosition(forYouText.getPosition().x, 230.0f);
    } else {
        forYouText.setFillColor(sf::Color(255, 255, 255, 150));
        followingText.setFillColor(sf::Color(0, 166, 81));
        tabIndicator.setSize(sf::Vector2f(followingText.getGlobalBounds().width, 3.0f));
        tabIndicator.setPosition(followingText.getPosition().x, 230.0f);
    }

    float y = 10.0f;
    for (const auto& post : sortedPosts) {
        // Filter Following tab: only posts by followed users (or self)
        if (activeTab == 1 && currentUser != nullptr) {
            std::string author = post.getAuthorUsername();
            if (author != currentUser->getUsername() && !socialGraph.isFollowing(currentUser->getUsername(), author)) {
                continue;
            }
        }

        // Check if the current user liked this post
        // Since we don't store liked-by-user mapping in Phase 3/4 yet, we can check if likeCount > 0 as a visual fallback or pass false.
        // Let's pass true/false. We can determine if liked by session user:
        // We'll set liked state depending on local toggleLike calls, but we can default liked status to false initially.
        bool liked = false; 

        auto card = std::make_unique<PostCard>(post, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 120.0f), liked);
        y += card->getHeight() + 15.0f;
        postCards.push_back(std::move(card));
    }

    maxScrollOffset = std::max(0.0f, y - 460.0f);
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

    // 3. Draw Tabs UI
    window.draw(forYouText);
    window.draw(followingText);
    window.draw(tabIndicator);

    // 4. Draw scrollable feed area with clipping view
    sf::View originalView = window.getView();
    window.setView(feedViewport);
    for (const auto& card : postCards) {
        card->draw(window);
    }
    window.setView(originalView);
}

void FeedScreen::handleEvent(sf::Event& event) {
    // Pass events to compose input and button
    composeInput->handleEvent(event);
    postButton->handleEvent(event);

    // Handle mouse move highlights
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        
        // Logout button hover
        if (logoutText.getGlobalBounds().contains(mousePos)) {
            logoutText.setFillColor(sf::Color(220, 53, 69, 255));
        } else {
            logoutText.setFillColor(sf::Color(220, 53, 69, 180));
        }

        // Tabs hover
        if (forYouText.getGlobalBounds().contains(mousePos) && activeTab != 0) {
            forYouText.setFillColor(sf::Color::White);
        } else if (activeTab != 0) {
            forYouText.setFillColor(sf::Color(255, 255, 255, 150));
        }

        if (followingText.getGlobalBounds().contains(mousePos) && activeTab != 1) {
            followingText.setFillColor(sf::Color::White);
        } else if (activeTab != 1) {
            followingText.setFillColor(sf::Color(255, 255, 255, 150));
        }
    }

    // Handle tab click switching
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        if (forYouText.getGlobalBounds().contains(mousePos) && activeTab != 0) {
            activeTab = 0;
            reloadFeed();
            return;
        }
        if (followingText.getGlobalBounds().contains(mousePos) && activeTab != 1) {
            activeTab = 1;
            reloadFeed();
            return;
        }
    }

    // Handle posting action
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
        scrollOffset -= event.mouseWheelScroll.delta * 40.0f;
        if (scrollOffset < 0.0f) scrollOffset = 0.0f;
        if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;
        
        feedViewport.setCenter(540.0f, 230.0f + scrollOffset);
    }

    // Map screen mouse coords to feedViewport coordinates for scrollable cards
    sf::Event mappedEvent = event;
    bool hasMouse = false;
    
    // Viewport bounds: X [100, 1180], Y [240, 700]
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        float sx = static_cast<float>(event.mouseButton.x);
        float sy = static_cast<float>(event.mouseButton.y);
        mappedEvent.mouseButton.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseButton.y = static_cast<int>((sy - 240.0f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        float sx = static_cast<float>(event.mouseMove.x);
        float sy = static_cast<float>(event.mouseMove.y);
        mappedEvent.mouseMove.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseMove.y = static_cast<int>((sy - 240.0f) + scrollOffset);
        hasMouse = true;
    }

    // Pass mapped events to scrollable post cards
    for (auto& card : postCards) {
        card->handleEvent(mappedEvent);
        
        // Handle username clicks -> routes to ProfileScreen
        if (hasMouse && card->isHandleClicked(mappedEvent)) {
            clickedHandle = card->getAuthorUsername();
        }

        // Handle like button clicks
        if (hasMouse && card->isLikeClicked(mappedEvent)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
        }
    }
}

std::string FeedScreen::getClickedHandle() {
    return clickedHandle;
}

void FeedScreen::clearClickedHandle() {
    clickedHandle = "";
}

bool FeedScreen::isLogoutClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return logoutText.getGlobalBounds().contains(mousePos);
    }
    return false;
}
