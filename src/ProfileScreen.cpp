#include "ProfileScreen.h"
#include "GlassPanel.h"
#include "Theme.h"
#include <algorithm>
#include <iostream>
#include <cctype>

ProfileScreen::ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm, CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg), currentUser(nullptr),
      targetUser(nullptr), scrollOffset(0.0f), targetScrollOffset(0.0f), maxScrollOffset(0.0f), clickedHandle(""), clickedPostId(-1) {

    // Configure header top bar (identical to FeedScreen)
    headerBackground.setPosition(0.0f, 0.0f);
    headerBackground.setSize(sf::Vector2f(1280.0f, 60.0f));

    // App logo
    logoText.setFont(font);
    logoText.setCharacterSize(24);
    logoText.setStyle(sf::Text::Bold);
    logoText.setFillColor(Theme::GREEN_PRIMARY);
    logoText.setString("PakistanHub");
    logoText.setPosition(40.0f, 15.0f);

    // Status text
    statusText.setFont(font);
    statusText.setCharacterSize(13);
    statusText.setFillColor(Theme::TEXT_MUTED);
    statusText.setPosition(200.0f, 22.0f);

    // Navigation buttons
    navHome = std::make_unique<GlassButton>(sf::Vector2f(750.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Home", GlassButton::Type::NAV_DEFAULT);
    navSearch = std::make_unique<GlassButton>(sf::Vector2f(860.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Search", GlassButton::Type::NAV_DEFAULT);
    navProfile = std::make_unique<GlassButton>(sf::Vector2f(970.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Profile", GlassButton::Type::NAV_ACTIVE);
    navLogout = std::make_unique<GlassButton>(sf::Vector2f(1080.0f, 12.0f), sf::Vector2f(100.0f, 36.0f), font, "Logout", GlassButton::Type::NAV_DEFAULT);

    // Profile header card background
    headerCard.setPosition(100.0f, 75.0f);
    headerCard.setSize(sf::Vector2f(1080.0f, 140.0f));
    headerCard.setFillColor(sf::Color(255, 255, 255, 30)); // Frosted panel (30 alpha)
    headerCard.setOutlineColor(sf::Color(255, 255, 255, 60)); // 60 alpha
    headerCard.setOutlineThickness(1.0f);

    // Nav accent line
    navAccentLine.setPosition(0.0f, 60.0f);
    navAccentLine.setSize(sf::Vector2f(1280.0f, 1.0f));
    navAccentLine.setFillColor(Theme::GLASS_BORDER);

    // Avatar circle on profile header
    avatarCircle.setRadius(30.0f);
    avatarCircle.setPosition(120.0f, 85.0f);
    avatarCircle.setFillColor(sf::Color(0, 166, 81, 25));
    avatarCircle.setOutlineColor(sf::Color(0, 166, 81, 200));
    avatarCircle.setOutlineThickness(2.0f);

    avatarLetter.setFont(font);
    avatarLetter.setCharacterSize(26);
    avatarLetter.setStyle(sf::Text::Bold);
    avatarLetter.setFillColor(sf::Color::White);

    // Setup empty state elements
    emptyCard.setSize(sf::Vector2f(600.0f, 80.0f));
    emptyCard.setPosition(240.0f, 150.0f);
    emptyCard.setFillColor(sf::Color(255, 255, 255, 30));
    emptyCard.setOutlineColor(sf::Color(255, 255, 255, 60));
    emptyCard.setOutlineThickness(1.0f);

    emptyText.setFont(font);
    emptyText.setCharacterSize(15);
    emptyText.setFillColor(sf::Color(255, 255, 255, 200));
    emptyText.setString("No posts yet.");
    sf::FloatRect etBounds = emptyText.getLocalBounds();
    emptyText.setOrigin(etBounds.left + etBounds.width / 2.0f, etBounds.top + etBounds.height / 2.0f);
    emptyText.setPosition(540.0f, 190.0f); // Center relative to viewport coordinates (540, 190)

    // Profile Display Name (shifted right for avatar)
    nameText.setFont(font);
    nameText.setCharacterSize(22);
    nameText.setStyle(sf::Text::Bold);
    nameText.setFillColor(Theme::TEXT_PRIMARY);
    nameText.setPosition(195.0f, 88.0f);

    // Profile Handle (@username)
    handleText.setFont(font);
    handleText.setCharacterSize(14);
    handleText.setFillColor(Theme::TEXT_MUTED);

    // Profile City
    cityText.setFont(font);
    cityText.setCharacterSize(14);
    cityText.setFillColor(Theme::TEXT_MUTED);
    cityText.setPosition(195.0f, 115.0f);

    // Profile Bio
    bioText.setFont(font);
    bioText.setCharacterSize(14);
    bioText.setFillColor(Theme::TEXT_PRIMARY);
    bioText.setPosition(195.0f, 138.0f);

    // Profile Stats counts
    statsText.setFont(font);
    statsText.setCharacterSize(14);
    statsText.setStyle(sf::Text::Bold);
    statsText.setFillColor(Theme::GREEN_PRIMARY);
    statsText.setPosition(195.0f, 168.0f);

    // Follow Button
    followButton = std::make_unique<GlassButton>(
        sf::Vector2f(940.0f, 120.0f),
        sf::Vector2f(220.0f, 50.0f),
        font, "FOLLOW"
    );

    // Setup scrollable view (clips inside y = 230 to y = 700)
    feedViewport.setSize(1080.0f, 470.0f);
    feedViewport.setViewport(sf::FloatRect(100.0f / 1280.0f, 230.0f / 720.0f, 1080.0f / 1280.0f, 470.0f / 720.0f));
    feedViewport.setCenter(540.0f, 235.0f);
}

void ProfileScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser != nullptr) {
        statusText.setString("Logged in as @" + currentUser->getUsername());
    }
}

void ProfileScreen::setTargetUser(User* user) {
    targetUser = user;
    reloadProfile();
}

void ProfileScreen::reloadProfile() {
    postCards.clear();
    scrollOffset = 0.0f;
    targetScrollOffset = 0.0f;
    clickedPostId = -1;
    feedViewport.setCenter(540.0f, 235.0f);
    clickedHandle = "";

    if (targetUser == nullptr) return;

    // Set textual details
    nameText.setString(targetUser->getDisplayName());
    
    handleText.setString("@" + targetUser->getUsername());
    float nameWidth = nameText.getGlobalBounds().width;
    handleText.setPosition(nameText.getPosition().x + nameWidth + 10.0f, nameText.getPosition().y + 6.0f);

    std::string cityStr = targetUser->getCity();
    cityText.setString(cityStr.empty() ? "📍 Unknown City" : "📍 " + cityStr);
    bioText.setString(targetUser->getBio().empty() ? "No bio written yet." : targetUser->getBio());

    // Friends count
    int friendsCount = static_cast<int>(socialGraph.getFriends(targetUser->getUsername()).size());
    statsText.setString(std::to_string(targetUser->getFollowerCount()) + " Followers   |   " + std::to_string(targetUser->getFollowingCount()) + " Following   |   " + std::to_string(friendsCount) + " Friends");

    // Update avatar letter
    std::string initial(1, static_cast<char>(std::toupper(targetUser->getDisplayName()[0])));
    avatarLetter.setString(initial);
    sf::FloatRect alBounds = avatarLetter.getLocalBounds();
    avatarLetter.setOrigin(alBounds.left + alBounds.width / 2.0f, alBounds.top + alBounds.height / 2.0f);
    avatarLetter.setPosition(120.0f + 30.0f, 85.0f + 30.0f);

    // Configure Follow button based on relationship
    if (currentUser != nullptr && targetUser != nullptr) {
        if (currentUser->getUsername() == targetUser->getUsername()) {
            // Self profile: hide follow button offscreen
            followButton->setPosition(sf::Vector2f(3000.0f, 3000.0f));
        } else {
            // Other profile: position normally and set toggle state
            followButton->setPosition(sf::Vector2f(940.0f, 120.0f));
            if (socialGraph.isFollowing(currentUser->getUsername(), targetUser->getUsername())) {
                followButton = std::make_unique<GlassButton>(sf::Vector2f(940.0f, 120.0f), sf::Vector2f(220.0f, 50.0f), font, "UNFOLLOW");
            } else {
                followButton = std::make_unique<GlassButton>(sf::Vector2f(940.0f, 120.0f), sf::Vector2f(220.0f, 50.0f), font, "FOLLOW");
            }
        }
    }

    // Load target user's posts
    std::vector<Post> userPosts = postManager.getPostsByUser(targetUser->getUsername());
    
    // Sort posts: newest first
    std::sort(userPosts.begin(), userPosts.end(), [](const Post& a, const Post& b) {
        if (a.getTimestamp() == b.getTimestamp()) {
            return a.getPostId() > b.getPostId();
        }
        return a.getTimestamp() > b.getTimestamp();
    });

    float y = 10.0f;
    for (const auto& post : userPosts) {
        int commentCount = commentManager.getCommentCountForPost(post.getPostId());
        auto card = std::make_unique<PostCard>(post, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 120.0f), false, commentCount);
        y += card->getHeight() + 15.0f;
        postCards.push_back(std::move(card));
    }

    maxScrollOffset = std::max(0.0f, y - 470.0f);
}

void ProfileScreen::draw(sf::RenderWindow& window) {
    // 1. Draw static Header
    GlassPanel::draw(window, headerBackground.getGlobalBounds(), false, 1.0f);
    window.draw(logoText);
    window.draw(statusText);
    navHome->draw(window);
    navSearch->draw(window);
    navProfile->draw(window);
    navLogout->draw(window);
    window.draw(navAccentLine);

    // 2. Draw profile header card
    GlassPanel::draw(window, headerCard.getGlobalBounds(), false, 1.0f);
    window.draw(avatarCircle);
    window.draw(avatarLetter);
    window.draw(nameText);
    window.draw(handleText);
    window.draw(cityText);
    window.draw(bioText);
    window.draw(statsText);

    // Draw follow button if not viewing self
    if (currentUser != nullptr && targetUser != nullptr && currentUser->getUsername() != targetUser->getUsername()) {
        followButton->draw(window);
    }

    // Draw scrollable posts
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

void ProfileScreen::handleEvent(sf::Event& event) {
    if (currentUser == nullptr || targetUser == nullptr) return;

    // Delegate events to top nav buttons
    navHome->handleEvent(event);
    navSearch->handleEvent(event);
    navProfile->handleEvent(event);
    navLogout->handleEvent(event);

    // Handle Follow/Unfollow button action
    if (currentUser->getUsername() != targetUser->getUsername()) {
        followButton->handleEvent(event);

        if (followButton->isClicked(event)) {
            bool following = socialGraph.isFollowing(currentUser->getUsername(), targetUser->getUsername());
            if (following) {
                // Unfollow relationship
                socialGraph.unfollow(currentUser->getUsername(), targetUser->getUsername());
                targetUser->setFollowerCount(std::max(0, targetUser->getFollowerCount() - 1));
                currentUser->setFollowingCount(std::max(0, currentUser->getFollowingCount() - 1));
            } else {
                // Follow relationship
                socialGraph.follow(currentUser->getUsername(), targetUser->getUsername());
                targetUser->setFollowerCount(targetUser->getFollowerCount() + 1);
                currentUser->setFollowingCount(currentUser->getFollowingCount() + 1);
            }

            // Save user stats update
            userManager.saveToFile("data/users.txt");
            reloadProfile();
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

    // Viewport bounds: X [100, 1180], Y [230, 700]
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        float sx = static_cast<float>(event.mouseButton.x);
        float sy = static_cast<float>(event.mouseButton.y);
        mappedEvent.mouseButton.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseButton.y = static_cast<int>((sy - 230.0f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        float sx = static_cast<float>(event.mouseMove.x);
        float sy = static_cast<float>(event.mouseMove.y);
        mappedEvent.mouseMove.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseMove.y = static_cast<int>((sy - 230.0f) + scrollOffset);
        hasMouse = true;
    }

    // Pass mapped events to scrollable post cards
    for (auto& card : postCards) {
        card->handleEvent(mappedEvent);

        // Click username to jump profiles
        if (hasMouse && card->isHandleClicked(mappedEvent)) {
            clickedHandle = card->getAuthorUsername();
        }

        // Handle comment clicks -> routes to PostDetailScreen
        if (hasMouse && card->isCommentClicked(mappedEvent)) {
            clickedPostId = card->getPostId();
        }

        // Like button clicks
        if (hasMouse && card->isLikeClicked(mappedEvent)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
        }
    }
}

void ProfileScreen::update() {
    float lerpFactor = 0.15f;
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f) {
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpFactor;
    } else {
        scrollOffset = targetScrollOffset;
    }
    feedViewport.setCenter(540.0f, 235.0f + scrollOffset);
}

bool ProfileScreen::isBackClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window;
    return isHomeClicked(event);
}

bool ProfileScreen::isHomeClicked(sf::Event& event) {
    return navHome->isClicked(event);
}

bool ProfileScreen::isSearchClicked(sf::Event& event) {
    return navSearch->isClicked(event);
}

bool ProfileScreen::isProfileClicked(sf::Event& event) {
    return navProfile->isClicked(event);
}

bool ProfileScreen::isLogoutClicked(sf::Event& event) {
    return navLogout->isClicked(event);
}

std::string ProfileScreen::getClickedHandle() {
    return clickedHandle;
}

void ProfileScreen::clearClickedHandle() {
    clickedHandle = "";
}

int ProfileScreen::getClickedPostId() {
    return clickedPostId;
}

void ProfileScreen::clearClickedPostId() {
    clickedPostId = -1;
}
