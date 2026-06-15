#include "PostDetailScreen.h"
#include <algorithm>
#include <iostream>

PostDetailScreen::PostDetailScreen(sf::Font& fnt, UserManager& um, PostManager& pm, CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg), currentUser(nullptr),
      targetPostLiked(false), scrollOffset(0.0f), targetScrollOffset(0.0f), maxScrollOffset(0.0f) {

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

    // Reply area background
    replyBackground.setPosition(100.0f, 630.0f);
    replyBackground.setSize(sf::Vector2f(1080.0f, 80.0f));
    replyBackground.setFillColor(sf::Color(255, 255, 255, 30)); // Glassmorphic
    replyBackground.setOutlineColor(sf::Color(255, 255, 255, 60));
    replyBackground.setOutlineThickness(1.0f);

    // Reply Input
    replyInput = std::make_unique<TextInput>(
        sf::Vector2f(120.0f, 642.0f),
        sf::Vector2f(800.0f, 56.0f),
        font, "Write a reply..."
    );

    // Reply Button
    replyButton = std::make_unique<GlassButton>(
        sf::Vector2f(940.0f, 642.0f),
        sf::Vector2f(220.0f, 56.0f),
        font, "REPLY"
    );

    // Scrollable comments viewport bounds: X [100, 1180], Y [260, 620]
    commentViewport.setSize(1080.0f, 360.0f);
    commentViewport.setViewport(sf::FloatRect(100.0f / 1280.0f, 260.0f / 720.0f, 1080.0f / 1280.0f, 360.0f / 720.0f));
    commentViewport.setCenter(540.0f, 180.0f);
}

void PostDetailScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser != nullptr) {
        statusText.setString("Logged in as @" + currentUser->getUsername());
    }
}

void PostDetailScreen::setTargetPost(const Post& post, bool liked) {
    targetPost = post;
    targetPostLiked = liked;
    
    // Construct the main post card representation
    int count = commentManager.getCommentCountForPost(targetPost.getPostId());
    postCard = std::make_unique<PostCard>(targetPost, font, sf::Vector2f(100.0f, 75.0f), sf::Vector2f(1080.0f, 140.0f), targetPostLiked, count);
    
    reloadComments();
}

void PostDetailScreen::reloadComments() {
    commentCards.clear();
    scrollOffset = 0.0f;
    targetScrollOffset = 0.0f;
    commentViewport.setCenter(540.0f, 180.0f);

    std::vector<Comment> thread = commentManager.getCommentsForPost(targetPost.getPostId());
    
    float y = 10.0f;
    for (const auto& cmt : thread) {
        auto card = std::make_unique<CommentCard>(cmt, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 80.0f));
        y += card->getHeight() + 12.0f;
        commentCards.push_back(std::move(card));
    }

    maxScrollOffset = std::max(0.0f, y - 360.0f);
}

void PostDetailScreen::draw(sf::RenderWindow& window) {
    // 1. Draw header
    window.draw(headerBackground);
    window.draw(logoText);
    window.draw(statusText);
    navHome->draw(window);
    navSearch->draw(window);
    navProfile->draw(window);
    navLogout->draw(window);

    // 2. Draw Main PostCard
    if (postCard) {
        postCard->draw(window);
    }

    // 3. Draw Reply area
    window.draw(replyBackground);
    replyInput->draw(window);
    replyButton->draw(window);

    // 4. Draw Scrollable Comments
    sf::View originalView = window.getView();
    window.setView(commentViewport);
    for (const auto& card : commentCards) {
        card->draw(window);
    }
    window.setView(originalView);
}

void PostDetailScreen::handleEvent(sf::Event& event) {
    // Pass events to nav buttons, input and reply button
    navHome->handleEvent(event);
    navSearch->handleEvent(event);
    navProfile->handleEvent(event);
    navLogout->handleEvent(event);

    replyInput->handleEvent(event);
    replyButton->handleEvent(event);

    // Like clicks on target post card
    if (postCard) {
        postCard->handleEvent(event);
        if (postCard->isLikeClicked(event)) {
            postManager.toggleLike(postCard->getPostId(), postCard->getIsLiked());
            targetPostLiked = postCard->getIsLiked();
        }
    }

    // Submit a new reply comment
    if (currentUser != nullptr && replyButton->isClicked(event)) {
        std::string text = replyInput->getText();
        if (!text.empty()) {
            commentManager.addComment(targetPost.getPostId(), currentUser->getUsername(), text);
            commentManager.saveToFile("data/comments.txt");
            replyInput->clear();
            
            // Reload comments and update the target postCard's comment count indicator
            reloadComments();
            int count = commentManager.getCommentCountForPost(targetPost.getPostId());
            postCard = std::make_unique<PostCard>(targetPost, font, sf::Vector2f(100.0f, 75.0f), sf::Vector2f(1080.0f, 140.0f), targetPostLiked, count);
        }
    }

    // Scroll wheel adjustment
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 50.0f;
        if (targetScrollOffset < 0.0f) targetScrollOffset = 0.0f;
        if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
    }

    // Map events inside scroll viewport
    sf::Event mappedEvent = event;
    // Viewport bounds: X [100, 1180], Y [260, 620]
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        float sx = static_cast<float>(event.mouseButton.x);
        float sy = static_cast<float>(event.mouseButton.y);
        mappedEvent.mouseButton.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseButton.y = static_cast<int>((sy - 260.0f) + scrollOffset);
    } else if (event.type == sf::Event::MouseMoved) {
        float sx = static_cast<float>(event.mouseMove.x);
        float sy = static_cast<float>(event.mouseMove.y);
        mappedEvent.mouseMove.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseMove.y = static_cast<int>((sy - 260.0f) + scrollOffset);
    }

    // Pass mapped events to scrollable comment cards
    for (auto& card : commentCards) {
        card->handleEvent(mappedEvent);
    }
}

void PostDetailScreen::update() {
    if (replyInput && replyButton) {
        bool hasContent = false;
        std::string text = replyInput->getText();
        for (char c : text) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasContent = true;
                break;
            }
        }
        replyButton->setEnabled(hasContent);
    }

    float lerpFactor = 0.15f;
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f) {
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpFactor;
    } else {
        scrollOffset = targetScrollOffset;
    }
    commentViewport.setCenter(540.0f, 180.0f + scrollOffset);
}

bool PostDetailScreen::isHomeClicked(sf::Event& event) {
    return navHome->isClicked(event);
}

bool PostDetailScreen::isSearchClicked(sf::Event& event) {
    return navSearch->isClicked(event);
}

bool PostDetailScreen::isProfileClicked(sf::Event& event) {
    return navProfile->isClicked(event);
}

bool PostDetailScreen::isLogoutClicked(sf::Event& event) {
    return navLogout->isClicked(event);
}
