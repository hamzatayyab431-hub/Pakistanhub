#include "SearchScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include <algorithm>
#include <cctype>

static bool ciContains(const std::string& str, const std::string& q) {
    if (q.empty()) return true;
    auto it = std::search(str.begin(), str.end(), q.begin(), q.end(),
        [](char a, char b){ return std::tolower(a)==std::tolower(b); });
    return it != str.end();
}

// ─── SearchResultCard ────────────────────────────────────────────────────────

SearchResultCard::SearchResultCard(User* usr, sf::Font& fnt,
                                   const sf::Vector2f& pos, const sf::Vector2f& sz)
    : user(usr), font(fnt), position(pos), size(sz)
{
    backgroundRect.setPosition(pos);
    backgroundRect.setSize(sz);
    hoverFill.speed = 12.f;
    hoverFill.snap(0.f);

    displayNameText.setFont(font);
    displayNameText.setCharacterSize(14);
    displayNameText.setStyle(sf::Text::Bold);
    displayNameText.setFillColor(Theme::TEXT_WHITE);
    displayNameText.setString(user->getDisplayName());
    displayNameText.setPosition(pos.x + 56.f, pos.y + 10.f);

    usernameText.setFont(font);
    usernameText.setCharacterSize(12);
    usernameText.setFillColor(Theme::TEXT_DIM);
    usernameText.setString("@" + user->getUsername());
    float nw = displayNameText.getGlobalBounds().width;
    usernameText.setPosition(pos.x + 56.f + nw + 6.f, pos.y + 12.f);

    statsText.setFont(font);
    statsText.setCharacterSize(12);
    statsText.setFillColor(Theme::TEXT_DIM);
    statsText.setString(std::to_string(user->getFollowerCount()) + " followers");
    sf::FloatRect sb = statsText.getLocalBounds();
    statsText.setPosition(pos.x + sz.x - sb.width - 16.f, pos.y + 12.f);

    avatarCenter = sf::Vector2f(pos.x + 8.f + 18.f, pos.y + sz.y / 2.f);
}

void SearchResultCard::draw(sf::RenderWindow& window) {
    sf::Uint8 ha = static_cast<sf::Uint8>(hoverFill.current * 2.f);
    sf::Color fill(255, 255, 255, ha);
    DrawUtils::drawRoundRect(window,
        sf::FloatRect(position.x, position.y, size.x, size.y),
        0.f, fill);
    DrawUtils::drawDivider(window, position.x, position.y + size.y - 1.f, size.x);

    if (!user->getDisplayName().empty())
        DrawUtils::drawAvatar(window, avatarCenter, 18.f,
            user->getDisplayName()[0], font, 14);

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
        if (isHovered != prev) hoverFill.setTarget(isHovered ? 30.f : 0.f);
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
    : font(fnt), userManager(um), postManager(pm), socialGraph(sg)
{
    sidebar.init(font, SidebarItem::EXPLORE, "", "");

    exploreTitle.setFont(font);
    exploreTitle.setCharacterSize(20);
    exploreTitle.setStyle(sf::Text::Bold);
    exploreTitle.setFillColor(Theme::TEXT_WHITE);
    exploreTitle.setString("Explore");
    exploreTitle.setPosition(CX + 16.f, 8.f);

    searchInput = std::make_unique<TextInput>(
        sf::Vector2f(CX + 8.f, 38.f), sf::Vector2f(CW - 16.f, 46.f),
        font, "Search PakistanHub...");

    noResultText.setFont(font);
    noResultText.setCharacterSize(16);
    noResultText.setFillColor(Theme::TEXT_DIM);
    noResultText.setString("No users found");
    sf::FloatRect nb = noResultText.getLocalBounds();
    noResultText.setOrigin(nb.left + nb.width / 2.f, 0.f);
    noResultText.setPosition(CW / 2.f, 60.f);

    resultsViewport.setSize(sf::Vector2f(CW, VP_H));
    resultsViewport.setViewport(sf::FloatRect(CX / 1280.f, VP_Y / 720.f,
                                              CW / 1280.f, VP_H / 720.f));
    resultsViewport.setCenter(CW / 2.f, VP_H / 2.f);
}

void SearchScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser)
        sidebar.setUsername(currentUser->getUsername(), currentUser->getDisplayName());
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
    resultsViewport.setCenter(CW / 2.f, VP_H / 2.f);

    const auto& allUsers = userManager.getUsers();
    float y = 0.f;
    for (const auto& usr : allUsers) {
        if (currentUser && usr.getUsername() == currentUser->getUsername()) continue;
        if (ciContains(usr.getUsername(), currentQuery) ||
            ciContains(usr.getDisplayName(), currentQuery)) {
            User* uptr = userManager.findUser(usr.getUsername());
            if (!uptr) continue;
            auto card = std::make_unique<SearchResultCard>(
                uptr, font, sf::Vector2f(0.f, y), sf::Vector2f(CW, 56.f));
            y += card->getHeight();
            resultCards.push_back(std::move(card));
        }
    }
    maxScrollOffset = std::max(0.f, y - VP_H);
}

void SearchScreen::draw(sf::RenderWindow& window) {
    sidebar.draw(window);

    // Center bg
    sf::RectangleShape centerBg(sf::Vector2f(CW, 720.f));
    centerBg.setPosition(CX, 0.f);
    centerBg.setFillColor(Theme::BG_PRIMARY);
    window.draw(centerBg);

    window.draw(exploreTitle);
    searchInput->draw(window);

    // Magnifier
    sf::Color iconCol = searchInput->getFocus() ? Theme::GREEN : Theme::TEXT_DIM;
    sf::CircleShape mag(9.f);
    mag.setPosition(CX + 14.f, 46.f);
    mag.setFillColor(sf::Color::Transparent);
    mag.setOutlineColor(iconCol);
    mag.setOutlineThickness(1.5f);
    window.draw(mag);
    sf::RectangleShape magHandle(sf::Vector2f(10.f, 1.5f));
    magHandle.setRotation(45.f);
    magHandle.setPosition(CX + 26.f, 62.f);
    magHandle.setFillColor(iconCol);
    window.draw(magHandle);

    DrawUtils::drawDivider(window, CX, VP_Y - 1.f, CW);

    sf::View orig = window.getView();
    window.setView(resultsViewport);
    if (resultCards.empty()) window.draw(noResultText);
    else for (const auto& card : resultCards) card->draw(window);
    window.setView(orig);
}

void SearchScreen::handleEvent(sf::Event& event) {
    sidebar.handleEvent(event);
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
        me.mouseButton.x = static_cast<int>(event.mouseButton.x - CX);
        me.mouseButton.y = static_cast<int>((event.mouseButton.y - VP_Y) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        me.mouseMove.x = static_cast<int>(event.mouseMove.x - CX);
        me.mouseMove.y = static_cast<int>((event.mouseMove.y - VP_Y) + scrollOffset);
        hasMouse = true;
    }

    for (auto& card : resultCards) {
        card->handleEvent(me);
        if (hasMouse && card->isClicked(me)) clickedHandle = card->getUsername();
    }
}

void SearchScreen::update(float dt) {
    sidebar.update(dt);
    searchInput->update(dt);

    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else scrollOffset = targetScrollOffset;
    resultsViewport.setCenter(CW / 2.f, VP_H / 2.f + scrollOffset);

    for (auto& card : resultCards) card->update(dt);
}

std::string SearchScreen::getClickedHandle()   { return clickedHandle; }
void        SearchScreen::clearClickedHandle() { clickedHandle = ""; }
