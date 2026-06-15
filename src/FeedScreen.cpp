#include "FeedScreen.h"
#include <algorithm>
#include <iostream>



FeedScreen::FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm, CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg), currentUser(nullptr),
      activeTab(0), scrollOffset(0.0f), targetScrollOffset(0.0f), maxScrollOffset(0.0f), clickedHandle(""), clickedPostId(-1) {

    // Configure header top bar
    headerBackground.setPosition(0.0f, 0.0f);
    headerBackground.setSize(sf::Vector2f(1280.0f, 60.0f));
    headerBackground.setFillColor(sf::Color(255, 255, 255, 30)); // Glassmorphic background
    headerBackground.setOutlineColor(sf::Color(255, 255, 255, 60)); // 60 alpha white
    headerBackground.setOutlineThickness(1.0f);

    // App logo
    logoText.setFont(font);
    logoText.setCharacterSize(22);
    logoText.setStyle(sf::Text::Bold);
    logoText.setFillColor(sf::Color(0, 166, 81)); // Pakistan Green #00A651
    logoText.setString("PakistanHub");
    logoText.setPosition(40.0f, 15.0f);

    // Status text
    statusText.setFont(font);
    statusText.setCharacterSize(13);
    statusText.setFillColor(sf::Color(255, 255, 255, 140));
    statusText.setPosition(200.0f, 22.0f);

    // Navigation buttons
    navHome = std::make_unique<GlassButton>(sf::Vector2f(750.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Home");
    navSearch = std::make_unique<GlassButton>(sf::Vector2f(860.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Search");
    navProfile = std::make_unique<GlassButton>(sf::Vector2f(970.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Profile");
    navLogout = std::make_unique<GlassButton>(sf::Vector2f(1080.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Logout");

    // Nav accent line
    navAccentLine.setPosition(0.0f, 60.0f);
    navAccentLine.setSize(sf::Vector2f(1280.0f, 2.0f));
    navAccentLine.setFillColor(sf::Color(0, 166, 81, 100));

    // Compose panel background
    composePanel.setPosition(100.0f, 80.0f);
    composePanel.setSize(sf::Vector2f(1080.0f, 110.0f));
    composePanel.setFillColor(sf::Color(255, 255, 255, 30)); // Glassmorphic panel
    composePanel.setOutlineColor(sf::Color(255, 255, 255, 60));
    composePanel.setOutlineThickness(1.0f);

    // Setup empty state elements
    emptyCard.setSize(sf::Vector2f(600.0f, 80.0f));
    emptyCard.setPosition(240.0f, 150.0f);
    emptyCard.setFillColor(sf::Color(255, 255, 255, 30));
    emptyCard.setOutlineColor(sf::Color(255, 255, 255, 60));
    emptyCard.setOutlineThickness(1.0f);

    emptyText.setFont(font);
    emptyText.setCharacterSize(15);
    emptyText.setFillColor(sf::Color(255, 255, 255, 200));
    emptyText.setString("No posts yet. Be the first to post!");
    sf::FloatRect etBounds = emptyText.getLocalBounds();
    emptyText.setOrigin(etBounds.left + etBounds.width / 2.0f, etBounds.top + etBounds.height / 2.0f);
    emptyText.setPosition(540.0f, 190.0f); // Center relative to viewport coordinates (540, 190)

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

    friendsText.setFont(font);
    friendsText.setCharacterSize(16);
    friendsText.setStyle(sf::Text::Bold);
    friendsText.setString("Friends");
    friendsText.setPosition(340.0f, 205.0f);

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
        statusText.setString("Logged in as @" + currentUser->getUsername());
    }
}

void FeedScreen::reloadFeed() {
    postCards.clear();
    scrollOffset = 0.0f;
    targetScrollOffset = 0.0f;
    clickedPostId = -1;
    feedViewport.setCenter(540.0f, 230.0f);

    // Load sorted posts
    std::vector<Post> sortedPosts = postManager.getAllPostsSorted();
    
    // Position tab indicator underline
    if (activeTab == 0) {
        forYouText.setFillColor(sf::Color(0, 166, 81));
        followingText.setFillColor(sf::Color(255, 255, 255, 150));
        friendsText.setFillColor(sf::Color(255, 255, 255, 150));
        tabIndicator.setSize(sf::Vector2f(forYouText.getGlobalBounds().width, 3.0f));
        tabIndicator.setPosition(forYouText.getPosition().x, 230.0f);
    } else if (activeTab == 1) {
        forYouText.setFillColor(sf::Color(255, 255, 255, 150));
        followingText.setFillColor(sf::Color(0, 166, 81));
        friendsText.setFillColor(sf::Color(255, 255, 255, 150));
        tabIndicator.setSize(sf::Vector2f(followingText.getGlobalBounds().width, 3.0f));
        tabIndicator.setPosition(followingText.getPosition().x, 230.0f);
    } else {
        forYouText.setFillColor(sf::Color(255, 255, 255, 150));
        followingText.setFillColor(sf::Color(255, 255, 255, 150));
        friendsText.setFillColor(sf::Color(0, 166, 81));
        tabIndicator.setSize(sf::Vector2f(friendsText.getGlobalBounds().width, 3.0f));
        tabIndicator.setPosition(friendsText.getPosition().x, 230.0f);
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
        // Filter Friends tab: only posts by mutual friends (or self)
        if (activeTab == 2 && currentUser != nullptr) {
            std::string author = post.getAuthorUsername();
            if (author != currentUser->getUsername() && !socialGraph.isFriend(currentUser->getUsername(), author)) {
                continue;
            }
        }

        // Check if the current user liked this post
        // Since we don't store liked-by-user mapping in Phase 3/4 yet, we can check if likeCount > 0 as a visual fallback or pass false.
        // Let's pass true/false. We can determine if liked by session user:
        // We'll set liked state depending on local toggleLike calls, but we can default liked status to false initially.
        bool liked = false; 
        int commentCount = commentManager.getCommentCountForPost(post.getPostId());

        auto card = std::make_unique<PostCard>(post, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 120.0f), liked, commentCount);
        y += card->getHeight() + 15.0f;
        postCards.push_back(std::move(card));
    }

    maxScrollOffset = std::max(0.0f, y - 460.0f);
}

void FeedScreen::draw(sf::RenderWindow& window) {
    // 1. Draw static Header
    window.draw(headerBackground);
    window.draw(logoText);
    window.draw(statusText);
    navHome->draw(window);
    navSearch->draw(window);
    navProfile->draw(window);
    navLogout->draw(window);
    window.draw(navAccentLine);

    // 2. Draw Compose area
    window.draw(composePanel);
    composeInput->draw(window);
    postButton->draw(window);

    // 3. Draw Tabs UI
    window.draw(forYouText);
    window.draw(followingText);
    window.draw(friendsText);
    window.draw(tabIndicator);

    // 4. Draw scrollable feed area with clipping view
    sf::View originalView = window.getView();
    window.setView(feedViewport);
    if (postCards.empty()) {
        window.draw(emptyCard);
        window.draw(emptyText);
    } else {
        for (const auto& card : postCards) {
            card->draw(window);
        }
    }
    window.setView(originalView);
}

void FeedScreen::handleEvent(sf::Event& event) {
    // Pass events to compose input, button and nav buttons
    composeInput->handleEvent(event);
    postButton->handleEvent(event);
    navHome->handleEvent(event);
    navSearch->handleEvent(event);
    navProfile->handleEvent(event);
    navLogout->handleEvent(event);

    // Handle mouse move highlights
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        
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

        if (friendsText.getGlobalBounds().contains(mousePos) && activeTab != 2) {
            friendsText.setFillColor(sf::Color::White);
        } else if (activeTab != 2) {
            friendsText.setFillColor(sf::Color(255, 255, 255, 150));
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
        if (friendsText.getGlobalBounds().contains(mousePos) && activeTab != 2) {
            activeTab = 2;
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

    // Scroll wheel target adjustment
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 60.0f;
        if (targetScrollOffset < 0.0f) targetScrollOffset = 0.0f;
        if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
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

        // Handle comment clicks -> routes to PostDetailScreen
        if (hasMouse && card->isCommentClicked(mappedEvent)) {
            clickedPostId = card->getPostId();
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

int FeedScreen::getClickedPostId() {
    return clickedPostId;
}

void FeedScreen::clearClickedPostId() {
    clickedPostId = -1;
}

void FeedScreen::update() {
    if (composeInput && postButton) {
        bool hasContent = false;
        std::string text = composeInput->getText();
        for (char c : text) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasContent = true;
                break;
            }
        }
        postButton->setEnabled(hasContent);
    }

    float lerpFactor = 0.15f;
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f) {
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpFactor;
    } else {
        scrollOffset = targetScrollOffset;
    }
    feedViewport.setCenter(540.0f, 230.0f + scrollOffset);
}

bool FeedScreen::isHomeClicked(sf::Event& event) {
    return navHome->isClicked(event);
}

bool FeedScreen::isSearchClicked(sf::Event& event) {
    return navSearch->isClicked(event);
}

bool FeedScreen::isProfileClicked(sf::Event& event) {
    return navProfile->isClicked(event);
}

bool FeedScreen::isLogoutClicked(sf::Event& event) {
    return navLogout->isClicked(event);
}
