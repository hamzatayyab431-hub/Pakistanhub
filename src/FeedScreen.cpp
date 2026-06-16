#include "FeedScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include "ToastManager.h"
#include <algorithm>
#include <cctype>

// ── Layout constants ────────────────────────────────────────────────────────
static constexpr float SB_W   = Theme::SIDEBAR_W;
static constexpr float CX     = Theme::CENTER_X;
static constexpr float CW     = Theme::CENTER_W;
static constexpr float FEED_VP_Y = 200.f;
static constexpr float FEED_VP_H = 720.f - FEED_VP_Y;

FeedScreen::FeedScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                       CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm),
      commentManager(cm), socialGraph(sg)
{
    sidebar.init(font, SidebarItem::HOME, "", "");
    rightPanel.init(font, um, sg);

    // Compose input — inside center column
    float inputX = CX + 56.f;   // avatar 38px + gap
    float inputW = CW - 56.f - 100.f;  // leave room for Post button

    composeInput = std::make_unique<TextInput>(
        sf::Vector2f(inputX, 20.f), sf::Vector2f(inputW, 44.f),
        font, "What's happening in Pakistan?...", false, 280);

    postButton = std::make_unique<GlassButton>(
        sf::Vector2f(CX + CW - 90.f, 24.f), sf::Vector2f(78.f, 34.f),
        font, "Post");

    charCountText.setFont(font);
    charCountText.setCharacterSize(11);
    charCountText.setFillColor(Theme::TEXT_DIM);

    // Tabs: For You / Following / Friends
    const char* tabLabels[3] = {"For You", "Following", "Friends"};
    float tabX = CX + 16.f;
    for (int i = 0; i < 3; ++i) {
        tabText[i].setFont(font);
        tabText[i].setCharacterSize(14);
        tabText[i].setStyle(sf::Text::Bold);
        tabText[i].setString(tabLabels[i]);
        tabText[i].setPosition(tabX, 155.f);
        tabX += tabText[i].getGlobalBounds().width + 32.f;
    }

    tabIndicator.setSize(sf::Vector2f(60.f, 3.f));
    tabIndicator.setFillColor(Theme::GREEN);
    tabIndicator.setPosition(CX + 16.f, 180.f);
    tabIndX.speed = 10.f;
    tabIndX.snap(CX + 16.f);

    // Scrollbar
    scrollTrack.setSize(sf::Vector2f(3.f, FEED_VP_H));
    scrollTrack.setPosition(CX + CW - 6.f, FEED_VP_Y);
    scrollTrack.setFillColor(Theme::DIVIDER);
    scrollThumb.setSize(sf::Vector2f(3.f, 60.f));
    scrollThumb.setPosition(CX + CW - 6.f, FEED_VP_Y);
    scrollThumb.setFillColor(Theme::GREEN_DIM);

    // Empty state
    emptyText.setFont(font);
    emptyText.setCharacterSize(18);
    emptyText.setStyle(sf::Text::Bold);
    emptyText.setFillColor(Theme::TEXT_MUTED);
    emptyText.setString("Nothing here yet");
    sf::FloatRect eb = emptyText.getLocalBounds();
    emptyText.setOrigin(eb.left + eb.width / 2.f, 0.f);
    emptyText.setPosition(CW / 2.f, 60.f);

    emptySubText.setFont(font);
    emptySubText.setCharacterSize(14);
    emptySubText.setFillColor(Theme::TEXT_DIM);
    emptySubText.setString("Be the first to post!");
    sf::FloatRect esb = emptySubText.getLocalBounds();
    emptySubText.setOrigin(esb.left + esb.width / 2.f, 0.f);
    emptySubText.setPosition(CW / 2.f, 88.f);

    // Feed viewport: clips center column below compose + tabs
    feedViewport.setSize(sf::Vector2f(CW, FEED_VP_H));
    feedViewport.setViewport(sf::FloatRect(
        CX / 1280.f, FEED_VP_Y / 720.f,
        CW / 1280.f, FEED_VP_H / 720.f));
    feedViewport.setCenter(CW / 2.f, FEED_VP_H / 2.f);
}

void FeedScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser) {
        sidebar.setUsername(currentUser->getUsername(), currentUser->getDisplayName());
        rightPanel.setCurrentUser(currentUser->getUsername());
    }
}

void FeedScreen::reloadFeed() {
    postCards.clear();
    scrollOffset = 0.f;
    targetScrollOffset = 0.f;
    clickedPostId = -1;
    feedViewport.setCenter(CW / 2.f, FEED_VP_H / 2.f);

    auto posts = postManager.getAllPostsSorted();
    float y = 8.f;
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
            post, font, sf::Vector2f(0.f, y), sf::Vector2f(CW, 120.f), false, cc);
        y += card->getHeight() + 1.f;   // 1px gap (cards share divider border)
        postCards.push_back(std::move(card));
    }
    maxScrollOffset = std::max(0.f, y - FEED_VP_H);
    updateScrollbar();
}

void FeedScreen::updateScrollbar() {
    float total = maxScrollOffset + FEED_VP_H;
    if (total <= 0.f) total = FEED_VP_H;
    float thumbH = std::max(30.f, FEED_VP_H * (FEED_VP_H / total));
    scrollThumb.setSize(sf::Vector2f(3.f, thumbH));
    float ratio = maxScrollOffset > 0.f ? scrollOffset / maxScrollOffset : 0.f;
    scrollThumb.setPosition(CX + CW - 6.f, FEED_VP_Y + ratio * (FEED_VP_H - thumbH));
}

void FeedScreen::updateCardPositions() {
    float y = 8.f;
    for (auto& card : postCards) { card->setPosition({0.f, y}); y += card->getHeight() + 1.f; }
}

void FeedScreen::draw(sf::RenderWindow& window) {
    // Sidebars
    sidebar.draw(window);
    rightPanel.draw(window);

    // Center column background
    sf::RectangleShape centerBg(sf::Vector2f(CW, 720.f));
    centerBg.setPosition(CX, 0.f);
    centerBg.setFillColor(Theme::BG_PRIMARY);
    window.draw(centerBg);

    // "Home" heading
    sf::Text homeTitle;
    homeTitle.setFont(font);
    homeTitle.setCharacterSize(20);
    homeTitle.setStyle(sf::Text::Bold);
    homeTitle.setFillColor(Theme::TEXT_WHITE);
    homeTitle.setString("Home");
    homeTitle.setPosition(CX + 16.f, 8.f);
    window.draw(homeTitle);

    // Compose area card
    float compH = composeHeightLerp.current;
    DrawUtils::drawCard(window,
        sf::FloatRect(CX, 38.f, CW, compH + 22.f),
        0.f, Theme::CARD_BG, Theme::DIVIDER, 1.f);

    // Avatar
    if (currentUser && !currentUser->getDisplayName().empty()) {
        DrawUtils::drawAvatar(window,
            sf::Vector2f(CX + 26.f, 38.f + (compH + 22.f) / 2.f),
            18.f, currentUser->getDisplayName()[0], font, 14);
    }
    composeInput->draw(window);
    postButton->draw(window);

    // Char count
    if (composeInput->getFocus()) {
        charCountText.setPosition(CX + CW - 90.f, 38.f + compH + 4.f);
        window.draw(charCountText);
    }

    // Bottom border of compose card
    DrawUtils::drawDivider(window, CX, 38.f + compH + 22.f, CW);

    // Tabs
    for (int i = 0; i < 3; ++i) {
        tabText[i].setFillColor(i == activeTab ? Theme::TEXT_WHITE : Theme::TEXT_DIM);
        window.draw(tabText[i]);
    }
    tabIndicator.setPosition(tabIndX.current, 180.f);
    window.draw(tabIndicator);

    DrawUtils::drawDivider(window, CX, 186.f, CW);

    // Feed
    sf::View orig = window.getView();
    window.setView(feedViewport);
    if (postCards.empty()) {
        window.draw(emptyText);
        window.draw(emptySubText);
    } else {
        for (const auto& card : postCards) card->draw(window);
    }
    window.setView(orig);

    if (maxScrollOffset > 0.f) {
        window.draw(scrollTrack);
        window.draw(scrollThumb);
    }
}

void FeedScreen::handleEvent(sf::Event& event) {
    sidebar.handleEvent(event);
    rightPanel.handleEvent(event);
    composeInput->handleEvent(event);
    postButton->handleEvent(event);

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

    // Post
    if (currentUser && postButton->isClicked(event)) {
        std::string content = composeInput->getText();
        bool hasContent = false;
        for (char c : content)
            if (!std::isspace(static_cast<unsigned char>(c))) { hasContent = true; break; }
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
        // Only scroll if mouse is in center column
        sf::Vector2f mp(static_cast<float>(event.mouseWheelScroll.x),
                        static_cast<float>(event.mouseWheelScroll.y));
        if (mp.x >= CX && mp.x < CX + CW && mp.y >= FEED_VP_Y) {
            targetScrollOffset -= event.mouseWheelScroll.delta * 60.f;
            if (targetScrollOffset < 0.f) targetScrollOffset = 0.f;
            if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
        }
    }

    // Map events for cards
    sf::Event me = event;
    bool hasMouse = false;
    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        float sx = static_cast<float>(event.mouseButton.x) - CX;
        float sy = static_cast<float>(event.mouseButton.y) - FEED_VP_Y + scrollOffset;
        me.mouseButton.x = static_cast<int>(sx);
        me.mouseButton.y = static_cast<int>(sy);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        float sx = static_cast<float>(event.mouseMove.x) - CX;
        float sy = static_cast<float>(event.mouseMove.y) - FEED_VP_Y + scrollOffset;
        me.mouseMove.x = static_cast<int>(sx);
        me.mouseMove.y = static_cast<int>(sy);
        hasMouse = true;
    }

    for (auto& card : postCards) {
        card->handleEvent(me);
        if (hasMouse && card->isHandleClicked(me)) clickedHandle = card->getAuthorUsername();
        if (hasMouse && card->isCommentClicked(me)) clickedPostId = card->getPostId();
        if (hasMouse && card->isLikeClicked(me)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
            if (card->getIsLiked())
                ToastManager::instance().push("Liked", "", ToastType::INFO);
        }
    }

    // Right panel suggestion clicks
    std::string rpHandle = rightPanel.getClickedHandle();
    if (!rpHandle.empty()) {
        rightPanel.clearClickedHandle();
        clickedHandle = rpHandle;
    }
}

void FeedScreen::update(float dt) {
    sidebar.update(dt);
    rightPanel.update(dt);
    composeInput->update(dt);
    postButton->update(dt);

    bool focused = composeInput->getFocus();
    composeHeightLerp.setTarget(focused ? 56.f : 40.f);
    composeHeightLerp.update(dt);
    composeHeight = composeHeightLerp.current;

    // Update compose input position to stay inside expand
    // (TextInput positioned at construction — just accept static layout)

    // Enable/disable post btn
    {
        std::string txt = composeInput->getText();
        bool hasC = false;
        for (char c : txt) if (!std::isspace(static_cast<unsigned char>(c))) { hasC = true; break; }
        postButton->setEnabled(hasC && txt.length() <= 280);
    }

    // Char counter
    int len = static_cast<int>(composeInput->getText().length());
    charCountText.setString(std::to_string(len) + "/280");
    if (len > 280) charCountText.setFillColor(Theme::ERROR);
    else if (len > 250) charCountText.setFillColor(Theme::WARNING);
    else charCountText.setFillColor(Theme::TEXT_DIM);

    // Tab indicator slide
    tabIndX.setTarget(tabText[activeTab].getPosition().x);
    tabIndX.update(dt);
    tabIndicator.setSize(sf::Vector2f(tabText[activeTab].getGlobalBounds().width, 3.f));

    // Scroll lerp
    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else scrollOffset = targetScrollOffset;
    feedViewport.setCenter(CW / 2.f, FEED_VP_H / 2.f + scrollOffset);

    updateScrollbar();
    for (auto& card : postCards) card->update(dt);
}

std::string FeedScreen::getClickedHandle()    { return clickedHandle; }
void        FeedScreen::clearClickedHandle()  { clickedHandle = ""; }
int         FeedScreen::getClickedPostId()    { return clickedPostId; }
void        FeedScreen::clearClickedPostId()  { clickedPostId = -1; }
