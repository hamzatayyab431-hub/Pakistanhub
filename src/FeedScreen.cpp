#include "FeedScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include "ToastManager.h"
#include <algorithm>
#include <iostream>

FeedScreen::FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                       CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm),
      commentManager(cm), socialGraph(sg), currentUser(nullptr),
      activeTab(0), scrollOffset(0.f), targetScrollOffset(0.f),
      maxScrollOffset(0.f), clickedHandle(""), clickedPostId(-1)
{
    nav.init(font, NavActive::HOME, "");

    // Compose panel
    composePanel.setPosition(100.f, 68.f);
    composePanel.setSize(sf::Vector2f(1080.f, 90.f));

    composeInput = std::make_unique<TextInput>(
        sf::Vector2f(155.f, 80.f), sf::Vector2f(750.f, 52.f),
        font, "What's on your mind?...", false, 280);

    postButton = std::make_unique<GlassButton>(
        sf::Vector2f(916.f, 80.f), sf::Vector2f(88.f, 34.f),
        font, "Post");

    charCountText.setFont(font);
    charCountText.setCharacterSize(12);
    charCountText.setFillColor(Theme::TEXT_DIM);
    charCountText.setPosition(916.f, 122.f);

    // Tabs
    const char* tabLabels[3] = {"For You", "Following", "Friends"};
    float tabX = 110.f;
    for (int i = 0; i < 3; ++i) {
        tabText[i].setFont(font);
        tabText[i].setCharacterSize(15);
        tabText[i].setStyle(sf::Text::Bold);
        tabText[i].setString(tabLabels[i]);
        tabText[i].setPosition(tabX, 172.f);
        tabX += tabText[i].getGlobalBounds().width + 28.f;
    }

    tabIndicator.setSize(sf::Vector2f(60.f, 3.f));
    tabIndicator.setFillColor(Theme::GREEN);
    tabIndicator.setPosition(110.f, 194.f);

    tabIndX.speed = 10.f;
    tabIndX.snap(110.f);

    // Scrollbar
    scrollTrack.setSize(sf::Vector2f(4.f, 460.f));
    scrollTrack.setPosition(1278.f, 205.f);
    scrollTrack.setFillColor(Theme::GLASS_BORDER);

    scrollThumb.setSize(sf::Vector2f(4.f, 60.f));
    scrollThumb.setPosition(1278.f, 205.f);
    scrollThumb.setFillColor(Theme::GREEN_DIM);

    // Empty state
    emptyText.setFont(font);
    emptyText.setCharacterSize(18);
    emptyText.setStyle(sf::Text::Bold);
    emptyText.setFillColor(Theme::TEXT_MUTED);
    emptyText.setString("Nothing here yet");
    sf::FloatRect eb = emptyText.getLocalBounds();
    emptyText.setOrigin(eb.left + eb.width / 2.f, 0.f);
    emptyText.setPosition(540.f, 80.f);

    emptySubText.setFont(font);
    emptySubText.setCharacterSize(14);
    emptySubText.setFillColor(Theme::TEXT_DIM);
    emptySubText.setString("Be the first to post!");
    sf::FloatRect esb = emptySubText.getLocalBounds();
    emptySubText.setOrigin(esb.left + esb.width / 2.f, 0.f);
    emptySubText.setPosition(540.f, 108.f);

    // Viewport: x[100,1180], y[205,665]
    feedViewport.setSize(1080.f, 460.f);
    feedViewport.setViewport(sf::FloatRect(100.f / 1280.f, 205.f / 720.f,
                                           1080.f / 1280.f, 460.f / 720.f));
    feedViewport.setCenter(540.f, 230.f);
}

void FeedScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser) nav.setUsername(currentUser->getUsername());
}

void FeedScreen::reloadFeed() {
    postCards.clear();
    scrollOffset = 0.f;
    targetScrollOffset = 0.f;
    clickedPostId = -1;
    feedViewport.setCenter(540.f, 230.f);

    std::vector<Post> posts = postManager.getAllPostsSorted();
    float y = 10.f;
    for (const auto& post : posts) {
        if (activeTab == 1 && currentUser) {
            std::string a = post.getAuthorUsername();
            if (a != currentUser->getUsername() &&
                !socialGraph.isFollowing(currentUser->getUsername(), a)) continue;
        }
        if (activeTab == 2 && currentUser) {
            std::string a = post.getAuthorUsername();
            if (a != currentUser->getUsername() &&
                !socialGraph.isFriend(currentUser->getUsername(), a)) continue;
        }
        int cc = commentManager.getCommentCountForPost(post.getPostId());
        auto card = std::make_unique<PostCard>(
            post, font, sf::Vector2f(0.f, y), sf::Vector2f(1080.f, 120.f), false, cc);
        y += card->getHeight() + 12.f;
        postCards.push_back(std::move(card));
    }
    maxScrollOffset = std::max(0.f, y - 460.f);
    updateScrollbar();
}

void FeedScreen::updateCardPositions() {
    float y = 10.f;
    for (auto& card : postCards) {
        card->setPosition(sf::Vector2f(0.f, y));
        y += card->getHeight() + 12.f;
    }
}

void FeedScreen::updateScrollbar() {
    float total = maxScrollOffset + 460.f;
    if (total <= 0.f) total = 460.f;
    float ratio = 460.f / total;
    float thumbH = std::max(30.f, 460.f * ratio);
    scrollThumb.setSize(sf::Vector2f(4.f, thumbH));

    float scrollRatio = maxScrollOffset > 0.f ? scrollOffset / maxScrollOffset : 0.f;
    float thumbY = 205.f + scrollRatio * (460.f - thumbH);
    scrollThumb.setPosition(1278.f, thumbY);
}

void FeedScreen::draw(sf::RenderWindow& window) {
    // Nav bar
    nav.draw(window);

    // Compose panel
    DrawUtils::drawGlassPanel(window,
        sf::FloatRect(100.f, 68.f, 1080.f, composeHeight), 6.f, Theme::GLASS_1);

    // Avatar in compose (left side)
    if (currentUser && !currentUser->getDisplayName().empty()) {
        DrawUtils::drawAvatar(window, sf::Vector2f(128.f, 68.f + composeHeight / 2.f),
            19.f, currentUser->getDisplayName()[0], font, 15);
    }

    composeInput->draw(window);
    postButton->draw(window);
    window.draw(charCountText);

    // Tabs
    for (int i = 0; i < 3; ++i) {
        int col = (activeTab == i) ? 0 : 1;
        tabText[i].setFillColor(col == 0 ? Theme::TEXT_WHITE : Theme::TEXT_MUTED);
        window.draw(tabText[i]);
    }
    // Tab indicator
    tabIndicator.setPosition(tabIndX.current, 194.f);
    window.draw(tabIndicator);

    // Feed scroll area
    sf::View orig = window.getView();
    window.setView(feedViewport);
    if (postCards.empty()) {
        // Empty state: speech bubble icon (simplified)
        sf::RectangleShape bubble(sf::Vector2f(64.f, 48.f));
        bubble.setOrigin(32.f, 24.f);
        bubble.setPosition(540.f, 46.f);
        bubble.setFillColor(sf::Color::Transparent);
        bubble.setOutlineColor(Theme::TEXT_DIM);
        bubble.setOutlineThickness(2.f);
        window.draw(bubble);
        // Nub
        sf::RectangleShape nub(sf::Vector2f(10.f, 8.f));
        nub.setPosition(508.f, 68.f);
        nub.setFillColor(sf::Color::Transparent);
        nub.setOutlineColor(Theme::TEXT_DIM);
        nub.setOutlineThickness(2.f);
        window.draw(nub);

        window.draw(emptyText);
        window.draw(emptySubText);
    } else {
        for (const auto& card : postCards) card->draw(window);
    }
    window.setView(orig);

    // Scrollbar
    if (maxScrollOffset > 0.f) {
        window.draw(scrollTrack);
        window.draw(scrollThumb);
    }
}

void FeedScreen::handleEvent(sf::Event& event) {
    nav.handleEvent(event);
    composeInput->handleEvent(event);
    postButton->handleEvent(event);

    // Char count update (handled in update())

    // Tab clicks
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        for (int i = 0; i < 3; ++i) {
            if (tabText[i].getGlobalBounds().contains(mp) && activeTab != i) {
                activeTab = i;
                reloadFeed();
                return;
            }
        }
    }

    // Post button
    if (currentUser && postButton->isClicked(event)) {
        std::string content = composeInput->getText();
        // Trim whitespace check
        bool hasContent = false;
        for (char c : content) {
            if (!std::isspace(static_cast<unsigned char>(c))) { hasContent = true; break; }
        }
        if (!hasContent) return;
        if (content.length() > 280) {
            ToastManager::instance().push("Too long", "Max 280 characters", ToastType::WARNING);
            return;
        }
        postManager.createPost(currentUser->getUsername(), content);
        postManager.saveToFile("data/posts.txt");
        composeInput->clear();
        reloadFeed();
        ToastManager::instance().push("Posted", "Your post is live", ToastType::SUCCESS);
        return;
    }

    // Scroll
    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 60.f;
        if (targetScrollOffset < 0.f) targetScrollOffset = 0.f;
        if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
    }

    // Map events for scrollable cards
    sf::Event me = event;
    bool hasMouse = false;
    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        me.mouseButton.x = static_cast<int>(event.mouseButton.x - 100.f);
        me.mouseButton.y = static_cast<int>((event.mouseButton.y - 205.f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        me.mouseMove.x = static_cast<int>(event.mouseMove.x - 100.f);
        me.mouseMove.y = static_cast<int>((event.mouseMove.y - 205.f) + scrollOffset);
        hasMouse = true;
    }

    for (auto& card : postCards) {
        card->handleEvent(me);
        if (hasMouse && card->isHandleClicked(me)) clickedHandle = card->getAuthorUsername();
        if (hasMouse && card->isCommentClicked(me)) clickedPostId = card->getPostId();
        if (hasMouse && card->isLikeClicked(me)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
            if (card->getIsLiked()) {
                ToastManager::instance().push("Liked", "", ToastType::INFO);
            }
        }
    }
}

void FeedScreen::update(float dt) {
    nav.update(dt);
    composeInput->update(dt);
    postButton->update(dt);

    // Compose expand
    bool focused = composeInput->getFocus();
    float targetH = focused ? 110.f : 90.f;
    composeHeightLerp.setTarget(targetH);
    composeHeightLerp.update(dt);
    composeHeight = composeHeightLerp.current;

    // Enable/disable post button
    {
        std::string txt = composeInput->getText();
        bool hasContent = false;
        for (char c : txt) {
            if (!std::isspace(static_cast<unsigned char>(c))) { hasContent = true; break; }
        }
        postButton->setEnabled(hasContent && txt.length() <= 280);
    }

    // Char counter
    int len = static_cast<int>(composeInput->getText().length());
    charCountText.setString(std::to_string(len) + " / 280");
    if (len > 280) charCountText.setFillColor(Theme::ERROR);
    else if (len > 250) charCountText.setFillColor(Theme::WARNING);
    else charCountText.setFillColor(Theme::TEXT_DIM);

    // Tab indicator slide
    float tabTargetX = tabText[activeTab].getPosition().x;
    float tabTargetW = tabText[activeTab].getGlobalBounds().width;
    tabIndX.setTarget(tabTargetX);
    tabIndX.update(dt);
    tabIndicator.setSize(sf::Vector2f(tabTargetW, 3.f));

    // Smooth scroll
    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else
        scrollOffset = targetScrollOffset;
    feedViewport.setCenter(540.f, 230.f + scrollOffset);

    // Update scrollbar position
    updateScrollbar();

    // Update post card animations
    for (auto& card : postCards) card->update(dt);
}

std::string FeedScreen::getClickedHandle()    { return clickedHandle; }
void        FeedScreen::clearClickedHandle()  { clickedHandle = ""; }
int         FeedScreen::getClickedPostId()    { return clickedPostId; }
void        FeedScreen::clearClickedPostId()  { clickedPostId = -1; }

bool FeedScreen::isHomeClicked(sf::Event& e)    { return nav.homeClicked(e); }
bool FeedScreen::isSearchClicked(sf::Event& e)  { return nav.searchClicked(e); }
bool FeedScreen::isProfileClicked(sf::Event& e) { return nav.profileClicked(e); }
bool FeedScreen::isLogoutClicked(sf::Event& e)  { return nav.logoutClicked(e); }
