#include "SearchScreen.h"
#include <algorithm>
#include <iostream>
#include <cctype>

// Helper to perform case-insensitive substring search
static bool containsIgnoreCase(const std::string& str, const std::string& query) {
    if (query.empty()) return true;
    auto it = std::search(
        str.begin(), str.end(),
        query.begin(), query.end(),
        [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); }
    );
    return it != str.end();
}

// ==================== SearchResultCard Implementation ====================

SearchResultCard::SearchResultCard(User* usr, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz)
    : user(usr), font(fnt), position(pos), size(sz), isHovered(false) {

    backgroundRect.setPosition(position);
    backgroundRect.setSize(size);
    backgroundRect.setFillColor(sf::Color(255, 255, 255, 30));
    backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 60));
    backgroundRect.setOutlineThickness(1.0f);

    displayNameText.setFont(font);
    displayNameText.setCharacterSize(16);
    displayNameText.setStyle(sf::Text::Bold);
    displayNameText.setFillColor(sf::Color::White);
    displayNameText.setString(user->getDisplayName());
    displayNameText.setPosition(position.x + 20.0f, position.y + 15.0f);

    usernameText.setFont(font);
    usernameText.setCharacterSize(13);
    usernameText.setFillColor(sf::Color(255, 255, 255, 140));
    usernameText.setString("@" + user->getUsername());
    float nameWidth = displayNameText.getGlobalBounds().width;
    usernameText.setPosition(position.x + 20.0f + nameWidth + 8.0f, position.y + 17.0f);

    statsText.setFont(font);
    statsText.setCharacterSize(12);
    statsText.setFillColor(sf::Color(0, 166, 81)); // Pakistan Green stats
    statsText.setString(std::to_string(user->getFollowerCount()) + " Followers  |  " + std::to_string(user->getFollowingCount()) + " Following");
    statsText.setPosition(position.x + 20.0f, position.y + 45.0f);
}

void SearchResultCard::draw(sf::RenderWindow& window) {
    if (isHovered) {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 50));
        backgroundRect.setOutlineColor(sf::Color(0, 166, 81, 200));
    } else {
        backgroundRect.setFillColor(sf::Color(255, 255, 255, 30));
        backgroundRect.setOutlineColor(sf::Color(255, 255, 255, 60));
    }

    window.draw(backgroundRect);
    window.draw(displayNameText);
    window.draw(usernameText);
    window.draw(statsText);
}

void SearchResultCard::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
        isHovered = backgroundRect.getGlobalBounds().contains(mousePos);
    }
}

bool SearchResultCard::isClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        return backgroundRect.getGlobalBounds().contains(mousePos);
    }
    return false;
}

std::string SearchResultCard::getUsername() const {
    return user->getUsername();
}

// ==================== SearchScreen Implementation ====================

SearchScreen::SearchScreen(sf::Font& fnt, UserManager& um, PostManager& pm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), socialGraph(sg), currentUser(nullptr),
      scrollOffset(0.0f), targetScrollOffset(0.0f), maxScrollOffset(0.0f), clickedHandle("") {

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

    // Search query TextInput
    searchInput = std::make_unique<TextInput>(
        sf::Vector2f(100.0f, 80.0f),
        sf::Vector2f(1080.0f, 45.0f),
        font, "Search by name or handle..."
    );

    // Viewport bounds: X [100, 1180], Y [145, 700] (Height 555)
    resultsViewport.setSize(1080.0f, 555.0f);
    resultsViewport.setViewport(sf::FloatRect(100.0f / 1280.0f, 145.0f / 720.0f, 1080.0f / 1280.0f, 555.0f / 720.0f));
    resultsViewport.setCenter(540.0f, 277.5f);
}

void SearchScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser != nullptr) {
        statusText.setString("Logged in as @" + currentUser->getUsername());
    }
}

void SearchScreen::reloadSearch() {
    searchInput->clear();
    currentQuery = "";
    clickedHandle = "";
    updateResults();
}

void SearchScreen::updateResults() {
    resultCards.clear();
    scrollOffset = 0.0f;
    targetScrollOffset = 0.0f;
    resultsViewport.setCenter(540.0f, 277.5f);

    const std::vector<User>& allUsers = userManager.getUsers();
    
    float y = 10.0f;
    for (auto& usr : allUsers) {
        // Exclude self from search results
        if (currentUser != nullptr && usr.getUsername() == currentUser->getUsername()) {
            continue;
        }

        // Apply live search filtering
        if (containsIgnoreCase(usr.getUsername(), currentQuery) || containsIgnoreCase(usr.getDisplayName(), currentQuery)) {
            // Find in-memory reference to user so counts are live
            User* userPtr = userManager.findUser(usr.getUsername());
            if (userPtr != nullptr) {
                auto card = std::make_unique<SearchResultCard>(userPtr, font, sf::Vector2f(0.0f, y), sf::Vector2f(1080.0f, 80.0f));
                y += card->getHeight() + 15.0f;
                resultCards.push_back(std::move(card));
            }
        }
    }

    maxScrollOffset = std::max(0.0f, y - 555.0f);
}

void SearchScreen::draw(sf::RenderWindow& window) {
    // 1. Draw static Header
    window.draw(headerBackground);
    window.draw(logoText);
    window.draw(statusText);
    navHome->draw(window);
    navSearch->draw(window);
    navProfile->draw(window);
    navLogout->draw(window);

    // 2. Draw search input
    searchInput->draw(window);

    // 3. Draw Results scroll area
    sf::View originalView = window.getView();
    window.setView(resultsViewport);
    for (const auto& card : resultCards) {
        card->draw(window);
    }
    window.setView(originalView);
}

void SearchScreen::handleEvent(sf::Event& event) {
    // Pass events to nav buttons and text inputs
    navHome->handleEvent(event);
    navSearch->handleEvent(event);
    navProfile->handleEvent(event);
    navLogout->handleEvent(event);

    searchInput->handleEvent(event);

    // If query text changed, run live search filter immediately
    if (searchInput->getText() != currentQuery) {
        currentQuery = searchInput->getText();
        updateResults();
    }

    // Scroll wheel adjustment
    if (event.type == sf::Event::MouseWheelScrolled && event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 50.0f;
        if (targetScrollOffset < 0.0f) targetScrollOffset = 0.0f;
        if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
    }

    // Map screen events inside results viewport
    sf::Event mappedEvent = event;
    bool hasMouse = false;
    // Viewport bounds: X [100, 1180], Y [145, 700]
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        float sx = static_cast<float>(event.mouseButton.x);
        float sy = static_cast<float>(event.mouseButton.y);
        mappedEvent.mouseButton.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseButton.y = static_cast<int>((sy - 145.0f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        float sx = static_cast<float>(event.mouseMove.x);
        float sy = static_cast<float>(event.mouseMove.y);
        mappedEvent.mouseMove.x = static_cast<int>(sx - 100.0f);
        mappedEvent.mouseMove.y = static_cast<int>((sy - 145.0f) + scrollOffset);
        hasMouse = true;
    }

    // Pass mapped events to scrollable result cards
    for (auto& card : resultCards) {
        card->handleEvent(mappedEvent);
        if (hasMouse && card->isClicked(mappedEvent)) {
            clickedHandle = card->getUsername();
        }
    }
}

void SearchScreen::update() {
    float lerpFactor = 0.15f;
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f) {
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpFactor;
    } else {
        scrollOffset = targetScrollOffset;
    }
    resultsViewport.setCenter(540.0f, 277.5f + scrollOffset);
}

std::string SearchScreen::getClickedHandle() {
    return clickedHandle;
}

void SearchScreen::clearClickedHandle() {
    clickedHandle = "";
}

bool SearchScreen::isHomeClicked(sf::Event& event) {
    return navHome->isClicked(event);
}

bool SearchScreen::isSearchClicked(sf::Event& event) {
    return navSearch->isClicked(event);
}

bool SearchScreen::isProfileClicked(sf::Event& event) {
    return navProfile->isClicked(event);
}

bool SearchScreen::isLogoutClicked(sf::Event& event) {
    return navLogout->isClicked(event);
}
