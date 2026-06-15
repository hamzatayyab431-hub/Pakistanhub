#include "SearchScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <algorithm>
#include <cctype>

static bool containsIgnoreCase(const std::string& str, const std::string& q) {
    if (q.empty()) return true;
    auto it = std::search(str.begin(), str.end(), q.begin(), q.end(),
        [](char a, char b) { return std::tolower(a) == std::tolower(b); });
    return it != str.end();
}

// ─── SearchResultCard ────────────────────────────────────────────────────────

SearchResultCard::SearchResultCard(User* usr, sf::Font& fnt,
                                   const sf::Vector2f& pos, const sf::Vector2f& sz)
    : user(usr), font(fnt), position(pos), size(sz), isHovered(false)
{
    backgroundRect.setPosition(pos);
    backgroundRect.setSize(sz);

    accentBar.setPosition(pos.x, pos.y);
    accentBar.setSize(sf::Vector2f(3.f, sz.y));
    accentBar.setFillColor(Theme::GREEN);

    displayNameText.setFont(font);
    displayNameText.setCharacterSize(15);
    displayNameText.setStyle(sf::Text::Bold);
    displayNameText.setFillColor(Theme::TEXT_WHITE);
    displayNameText.setString(user->getDisplayName());
    displayNameText.setPosition(pos.x + 68.f, pos.y + 12.f);

    usernameText.setFont(font);
    usernameText.setCharacterSize(13);
    usernameText.setFillColor(Theme::TEXT_MUTED);
    usernameText.setString("@" + user->getUsername());
    float nw = displayNameText.getGlobalBounds().width;
    usernameText.setPosition(pos.x + 68.f + nw + 8.f, pos.y + 14.f);

    statsText.setFont(font);
    statsText.setCharacterSize(12);
    statsText.setFillColor(Theme::TEXT_DIM);
    statsText.setString(std::to_string(user->getFollowerCount()) + " followers");
    sf::FloatRect sb = statsText.getLocalBounds();
    statsText.setPosition(pos.x + sz.x - sb.width - 16.f, pos.y + 14.f);

    hoverFill.speed = 10.f;
    hoverFill.snap(0.f);

    // Avatar center: 28px radius
    avatarCenter = sf::Vector2f(pos.x + 8.f + 20.f, pos.y + sz.y / 2.f);
}

void SearchResultCard::draw(sf::RenderWindow& window) {
    sf::Uint8 fillA = static_cast<sf::Uint8>(12 + hoverFill.current * 8.f);
    sf::Color fill(255, 255, 255, fillA);
    DrawUtils::drawGlassPanel(window,
        sf::FloatRect(position.x, position.y, size.x, size.y), 4.f, fill);

    window.draw(accentBar);

    if (!user->getDisplayName().empty()) {
        DrawUtils::drawAvatar(window, avatarCenter, 20.f,
            user->getDisplayName()[0], font, 16);
    }

    window.draw(displayNameText);
    window.draw(usernameText);
    window.draw(statsText);
}

void SearchResultCard::handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mp(static_cast<float>(event.mouseMove.x),
                        static_cast<float>(event.mouseMove.y));
        bool prev = isHovered;
        isHovered = backgroundRect.getGlobalBounds().contains(mp);
        if (isHovered != prev) hoverFill.setTarget(isHovered ? 2.f : 0.f);
    }
}

void SearchResultCard::update(float dt) { hoverFill.update(dt); }

bool SearchResultCard::isClicked(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        return backgroundRect.getGlobalBounds().contains(mp);
    }
    return false;
}

std::string SearchResultCard::getUsername() const { return user->getUsername(); }

// ─── SearchScreen ────────────────────────────────────────────────────────────

SearchScreen::SearchScreen(sf::Font& fnt, UserManager& um,
                           PostManager& pm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), socialGraph(sg),
      currentUser(nullptr), scrollOffset(0.f), targetScrollOffset(0.f),
      maxScrollOffset(0.f), clickedHandle("")
{
    nav.init(font, NavActive::SEARCH, "");

    searchInput = std::make_unique<TextInput>(
        sf::Vector2f(100.f, 72.f), sf::Vector2f(1080.f, 50.f),
        font, "Search by name or handle...");

    // Magnifier icon will be drawn manually in draw()

    // No-results text
    noResultText.setFont(font);
    noResultText.setCharacterSize(16);
    noResultText.setFillColor(Theme::TEXT_MUTED);
    noResultText.setString("No users found");
    sf::FloatRect nb = noResultText.getLocalBounds();
    noResultText.setOrigin(nb.left + nb.width / 2.f, 0.f);
    noResultText.setPosition(540.f, 60.f);

    // Viewport: x[100,1180], y[132,695]
    resultsViewport.setSize(1080.f, 563.f);
    resultsViewport.setViewport(sf::FloatRect(100.f / 1280.f, 132.f / 720.f,
                                              1080.f / 1280.f, 563.f / 720.f));
    resultsViewport.setCenter(540.f, 281.5f);
}

void SearchScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser) nav.setUsername(currentUser->getUsername());
}

void SearchScreen::reloadSearch() {
    searchInput->clear();
    currentQuery = "";
    clickedHandle = "";
    updateResults();
}

void SearchScreen::updateResults() {
    resultCards.clear();
    scrollOffset = 0.f;
    targetScrollOffset = 0.f;
    resultsViewport.setCenter(540.f, 281.5f);

    const auto& allUsers = userManager.getUsers();
    float y = 10.f;
    for (const auto& usr : allUsers) {
        if (currentUser && usr.getUsername() == currentUser->getUsername()) continue;
        if (containsIgnoreCase(usr.getUsername(),   currentQuery) ||
            containsIgnoreCase(usr.getDisplayName(), currentQuery)) {
            User* uptr = userManager.findUser(usr.getUsername());
            if (!uptr) continue;
            auto card = std::make_unique<SearchResultCard>(
                uptr, font, sf::Vector2f(0.f, y), sf::Vector2f(1080.f, 72.f));
            y += card->getHeight() + 10.f;
            resultCards.push_back(std::move(card));
        }
    }
    maxScrollOffset = std::max(0.f, y - 563.f);
}

void SearchScreen::draw(sf::RenderWindow& window) {
    nav.draw(window);
    searchInput->draw(window);

    // Magnifier icon (manual draw)
    sf::Color iconCol = searchInput->getFocus() ? Theme::GREEN : Theme::TEXT_DIM;
    sf::CircleShape magnCircle(8.f);
    magnCircle.setPosition(110.f, 82.f);
    magnCircle.setFillColor(sf::Color::Transparent);
    magnCircle.setOutlineColor(iconCol);
    magnCircle.setOutlineThickness(1.5f);
    window.draw(magnCircle);
    sf::RectangleShape magnHandle(sf::Vector2f(10.f, 1.5f));
    magnHandle.setRotation(45.f);
    magnHandle.setPosition(122.f, 97.f);
    magnHandle.setFillColor(iconCol);
    window.draw(magnHandle);

    sf::View orig = window.getView();
    window.setView(resultsViewport);
    if (resultCards.empty()) {
        window.draw(noResultText);
    } else {
        for (const auto& card : resultCards) card->draw(window);
    }
    window.setView(orig);
}

void SearchScreen::handleEvent(sf::Event& event) {
    nav.handleEvent(event);
    searchInput->handleEvent(event);

    if (searchInput->getText() != currentQuery) {
        currentQuery = searchInput->getText();
        updateResults();
    }

    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 50.f;
        if (targetScrollOffset < 0.f) targetScrollOffset = 0.f;
        if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
    }

    sf::Event me = event;
    bool hasMouse = false;
    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        me.mouseButton.x = static_cast<int>(event.mouseButton.x - 100.f);
        me.mouseButton.y = static_cast<int>((event.mouseButton.y - 132.f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        me.mouseMove.x = static_cast<int>(event.mouseMove.x - 100.f);
        me.mouseMove.y = static_cast<int>((event.mouseMove.y - 132.f) + scrollOffset);
        hasMouse = true;
    }

    for (auto& card : resultCards) {
        card->handleEvent(me);
        if (hasMouse && card->isClicked(me)) clickedHandle = card->getUsername();
    }
}

void SearchScreen::update(float dt) {
    nav.update(dt);
    searchInput->update(dt);

    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else
        scrollOffset = targetScrollOffset;
    resultsViewport.setCenter(540.f, 281.5f + scrollOffset);

    for (auto& card : resultCards) card->update(dt);
}

void SearchScreen::update() { update(0.016f); }

std::string SearchScreen::getClickedHandle()   { return clickedHandle; }
void        SearchScreen::clearClickedHandle() { clickedHandle = ""; }

bool SearchScreen::isHomeClicked(sf::Event& e)    { return nav.homeClicked(e); }
bool SearchScreen::isSearchClicked(sf::Event& e)  { return nav.searchClicked(e); }
bool SearchScreen::isProfileClicked(sf::Event& e) { return nav.profileClicked(e); }
bool SearchScreen::isLogoutClicked(sf::Event& e)  { return nav.logoutClicked(e); }
