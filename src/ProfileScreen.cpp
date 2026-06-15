#include "ProfileScreen.h"
#include <algorithm>
#include <iostream>

static void centerTextHorizontally(sf::Text& text, float centerX) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, 0.0f);
    text.setPosition(centerX, text.getPosition().y);
}

ProfileScreen::ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), socialGraph(sg), currentUser(nullptr),
      targetUser(nullptr), scrollOffset(0.0f), maxScrollOffset(0.0f), clickedHandle("") {

    // Configure Back Link
    backLink.setFont(font);
    backLink.setCharacterSize(16);
    backLink.setFillColor(sf::Color(0, 166, 81, 180)); // Muted Green
    backLink.setString("<- Back to Feed");
    backLink.setPosition(100.0f, 40.0f);

    // Profile header card background
    headerCard.setPosition(100.0f, 75.0f);
    headerCard.setSize(sf::Vector2f(1080.0f, 140.0f));
    headerCard.setFillColor(sf::Color(255, 255, 255, 20)); // Frosted panel
    headerCard.setOutlineColor(sf::Color(255, 255, 255, 60));
    headerCard.setOutlineThickness(1.0f);

    // Profile Display Name
    nameText.setFont(font);
    nameText.setCharacterSize(22);
    nameText.setStyle(sf::Text::Bold);
    nameText.setFillColor(sf::Color::White);
    nameText.setPosition(120.0f, 90.0f);

    // Profile Handle (@username)
    handleText.setFont(font);
    handleText.setCharacterSize(14);
    handleText.setFillColor(sf::Color(255, 255, 255, 140));

    // Profile Bio
    bioText.setFont(font);
    bioText.setCharacterSize(14);
    bioText.setFillColor(sf::Color(255, 255, 255, 180));
    bioText.setPosition(120.0f, 138.0f);

    // Profile Stats counts
    statsText.setFont(font);
    statsText.setCharacterSize(14);
    statsText.setStyle(sf::Text::Bold);
    statsText.setFillColor(sf::Color(0, 166, 81));
    statsText.setPosition(120.0f, 172.0f);

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
}

void ProfileScreen::setTargetUser(User* user) {
    targetUser = user;
    reloadProfile();
}

void ProfileScreen::reloadProfile() {
    postCards.clear();
    scrollOffset = 0.0f;
    feedViewport.setCenter(540.0f, 235.0f);
    clickedHandle = "";

    if (targetUser == nullptr) return;

    // Set textual details
    nameText.setString(targetUser->getDisplayName());
    
    handleText.setString("@" + targetUser->getUsername());
    float nameWidth = nameText.getGlobalBounds().width;
    handleText.setPosition(nameText.getPosition().x + nameWidth + 10.0f, nameText.getPosition().y + 6.0f);

    bioText.setString(targetUser->getBio().empty() ? "No bio written yet." : targetUser->getBio());
    statsText.setString(std::to_string(targetUser->getFollowerCount()) + " Followers   |   " + std::to_string(targetUser->getFollowingCount()) + " Following");

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
        auto card = std::make_unique<PostCard>(post, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 120.0f), false);
        y += card->getHeight() + 15.0f;
        postCards.push_back(std::move(card));
    }

    maxScrollOffset = std::max(0.0f, y - 470.0f);
}

void ProfileScreen::draw(sf::RenderWindow& window) {
    // Draw back link
    window.draw(backLink);

    // Draw header card panel
    window.draw(headerCard);
    window.draw(nameText);
    window.draw(handleText);
    window.draw(bioText);
    window.draw(statsText);

    // Draw follow button if not viewing self
    if (currentUser != nullptr && targetUser != nullptr && currentUser->getUsername() != targetUser->getUsername()) {
        followButton->draw(window);
    }

    // Draw scrollable posts
    sf::View originalView = window.getView();
    window.setView(feedViewport);
    for (const auto& card : postCards) {
        card->draw(window);
    }
    window.setView(originalView);
}

void ProfileScreen::handleEvent(sf::Event& event) {
    if (currentUser == nullptr || targetUser == nullptr) return;

    // Handle back button highlight
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        if (backLink.getGlobalBounds().contains(mousePos)) {
            backLink.setFillColor(sf::Color(0, 166, 81, 255)); // Highlight green
        } else {
            backLink.setFillColor(sf::Color(0, 166, 81, 180));
        }
    }

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

    // Scroll wheel offset adjustment
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        scrollOffset -= event.mouseWheelScroll.delta * 40.0f;
        if (scrollOffset < 0.0f) scrollOffset = 0.0f;
        if (scrollOffset > maxScrollOffset) scrollOffset = maxScrollOffset;
        
        feedViewport.setCenter(540.0f, 235.0f + scrollOffset);
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

        // Like button clicks
        if (hasMouse && card->isLikeClicked(mappedEvent)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
        }
    }
}

bool ProfileScreen::isBackClicked(sf::Event& event, sf::RenderWindow& window) {
    (void)window;
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return backLink.getGlobalBounds().contains(mousePos);
    }
    return false;
}

std::string ProfileScreen::getClickedHandle() {
    return clickedHandle;
}

void ProfileScreen::clearClickedHandle() {
    clickedHandle = "";
}
