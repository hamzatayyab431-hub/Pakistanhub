#include "ProfileScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include "ToastManager.h"
#include <algorithm>
#include <cctype>

ProfileScreen::ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                             CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg)
{
    sidebar.init(font, SidebarItem::PROFILE, "", "");

    coverBg.setPosition(CX, 0.f);
    coverBg.setSize(sf::Vector2f(CW, COVER_H));
    coverBg.setFillColor(Theme::BG_SURFACE);

    auto makeText = [&](sf::Text& t, unsigned int sz, sf::Color col, bool bold = false) {
        t.setFont(font); t.setCharacterSize(sz); t.setFillColor(col);
        if (bold) t.setStyle(sf::Text::Bold);
    };
    makeText(nameText,   20, Theme::TEXT_WHITE, true);
    makeText(handleText, 13, Theme::TEXT_DIM);
    makeText(bioText,    13, Theme::TEXT_MUTED);
    makeText(cityText,   12, Theme::TEXT_DIM);
    makeText(statsText,  13, Theme::TEXT_WHITE);

    followButton = std::make_unique<GlassButton>(
        sf::Vector2f(CX + CW - 130.f, COVER_H - 10.f), sf::Vector2f(110.f, 34.f),
        font, "Follow");
    editProfileButton = std::make_unique<GlassButton>(
        sf::Vector2f(CX + CW - 150.f, COVER_H - 10.f), sf::Vector2f(130.f, 34.f),
        font, "Edit Profile", GlassButton::Type::NAV_DEFAULT);
    saveProfileButton = std::make_unique<GlassButton>(
        sf::Vector2f(CX + CW - 130.f, COVER_H - 10.f), sf::Vector2f(110.f, 34.f),
        font, "Save");

    cityEditInput = std::make_unique<TextInput>(
        sf::Vector2f(CX + 16.f, COVER_H + 54.f), sf::Vector2f(200.f, 28.f),
        font, "City...");
    bioEditInput  = std::make_unique<TextInput>(
        sf::Vector2f(CX + 16.f, COVER_H + 86.f), sf::Vector2f(400.f, 28.f),
        font, "Bio...");

    // Tabs: Posts / Replies / Media
    const char* tabLabels[3] = {"Posts", "Replies", "Media"};
    float tx = CX + 16.f;
    for (int i = 0; i < 3; ++i) {
        tabText[i].setFont(font);
        tabText[i].setCharacterSize(14);
        tabText[i].setStyle(sf::Text::Bold);
        tabText[i].setString(tabLabels[i]);
        tabText[i].setPosition(tx, TAB_Y + 4.f);
        tx += tabText[i].getGlobalBounds().width + 28.f;
    }
    tabIndicator.setSize(sf::Vector2f(40.f, 3.f));
    tabIndicator.setFillColor(Theme::GREEN);
    tabIndicator.setPosition(CX + 16.f, TAB_Y + 28.f);
    tabIndX.speed = 10.f;
    tabIndX.snap(CX + 16.f);

    // Scrollbar
    scrollTrack.setSize(sf::Vector2f(3.f, VP_H));
    scrollTrack.setPosition(CX + CW - 6.f, VP_Y);
    scrollTrack.setFillColor(Theme::DIVIDER);
    scrollThumb.setSize(sf::Vector2f(3.f, 60.f));
    scrollThumb.setPosition(CX + CW - 6.f, VP_Y);
    scrollThumb.setFillColor(Theme::GREEN_DIM);

    emptyText.setFont(font);
    emptyText.setCharacterSize(16);
    emptyText.setFillColor(Theme::TEXT_DIM);
    emptyText.setString("No posts yet.");
    sf::FloatRect eb = emptyText.getLocalBounds();
    emptyText.setOrigin(eb.left + eb.width / 2.f, eb.top + eb.height / 2.f);
    emptyText.setPosition(CW / 2.f, 60.f);

    feedViewport.setSize(sf::Vector2f(CW, VP_H));
    feedViewport.setViewport(sf::FloatRect(CX / 1280.f, VP_Y / 720.f,
                                           CW / 1280.f, VP_H / 720.f));
    feedViewport.setCenter(CW / 2.f, VP_H / 2.f);
}

void ProfileScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser)
        sidebar.setUsername(currentUser->getUsername(), currentUser->getDisplayName());
}

void ProfileScreen::setTargetUser(User* user) {
    targetUser = user;
    isEditingProfile = false;
    reloadProfile();
}

void ProfileScreen::reloadProfile() {
    postCards.clear();
    scrollOffset = 0.f;
    targetScrollOffset = 0.f;
    clickedPostId = -1;
    clickedHandle = "";
    feedViewport.setCenter(CW / 2.f, VP_H / 2.f);
    if (!targetUser) return;

    nameText.setString(targetUser->getDisplayName());
    handleText.setString("@" + targetUser->getUsername());
    bioText.setString(targetUser->getBio().empty() ? "" : targetUser->getBio());
    cityText.setString(targetUser->getCity().empty() ? "" : "📍 " + targetUser->getCity());

    int friends = static_cast<int>(socialGraph.getFriends(targetUser->getUsername()).size());
    statsText.setString(
        std::to_string(targetUser->getFollowingCount()) + " Following   " +
        std::to_string(targetUser->getFollowerCount())  + " Followers   " +
        std::to_string(friends) + " Friends");

    bool isSelf = currentUser && currentUser->getUsername() == targetUser->getUsername();
    if (isSelf) {
        followButton->setPosition({3000.f, 3000.f});
        editProfileButton->setPosition(sf::Vector2f(CX + CW - 150.f, COVER_H - 10.f));
        saveProfileButton->setPosition({3000.f, 3000.f});
    } else {
        editProfileButton->setPosition({3000.f, 3000.f});
        saveProfileButton->setPosition({3000.f, 3000.f});
        bool following = currentUser &&
            socialGraph.isFollowing(currentUser->getUsername(), targetUser->getUsername());
        followButton = std::make_unique<GlassButton>(
            sf::Vector2f(CX + CW - 130.f, COVER_H - 10.f), sf::Vector2f(110.f, 34.f),
            font, following ? "Unfollow" : "Follow",
            following ? GlassButton::Type::NAV_DEFAULT : GlassButton::Type::PRIMARY);
    }

    auto userPosts = postManager.getPostsByUser(targetUser->getUsername());
    std::sort(userPosts.begin(), userPosts.end(), [](const Post& a, const Post& b){
        return a.getTimestamp() != b.getTimestamp()
            ? a.getTimestamp() > b.getTimestamp()
            : a.getPostId() > b.getPostId();
    });

    float y = 8.f;
    for (const auto& post : userPosts) {
        int cc = commentManager.getCommentCountForPost(post.getPostId());
        auto card = std::make_unique<PostCard>(
            post, font, sf::Vector2f(0.f, y), sf::Vector2f(CW, 120.f), false, cc);
        y += card->getHeight() + 1.f;
        postCards.push_back(std::move(card));
    }
    maxScrollOffset = std::max(0.f, y - VP_H);
}

void ProfileScreen::draw(sf::RenderWindow& window) {
    sidebar.draw(window);

    // Center bg
    sf::RectangleShape centerBg(sf::Vector2f(CW, 720.f));
    centerBg.setPosition(CX, 0.f);
    centerBg.setFillColor(Theme::BG_PRIMARY);
    window.draw(centerBg);

    // Cover gradient
    sf::VertexArray cover(sf::Quads, 4);
    cover[0].position = {CX, 0.f};            cover[0].color = Theme::BG_SURFACE;
    cover[1].position = {CX + CW, 0.f};       cover[1].color = Theme::BG_SURFACE;
    cover[2].position = {CX + CW, COVER_H};   cover[2].color = Theme::BG_SECONDARY;
    cover[3].position = {CX, COVER_H};        cover[3].color = Theme::BG_SECONDARY;
    window.draw(cover);

    // Avatar — 50px radius, overlaps cover
    bool isSelf = currentUser && targetUser &&
                  currentUser->getUsername() == targetUser->getUsername();
    if (targetUser && !targetUser->getDisplayName().empty()) {
        DrawUtils::drawAvatar(window, sf::Vector2f(CX + 56.f, COVER_H + 10.f),
            44.f, targetUser->getDisplayName()[0], font, 32);
    }

    // Profile info
    nameText.setPosition(CX + 16.f, COVER_H + 28.f);
    window.draw(nameText);

    float nw = nameText.getGlobalBounds().width;
    handleText.setPosition(CX + 16.f + nw + 8.f, COVER_H + 32.f);
    window.draw(handleText);

    bioText.setPosition(CX + 16.f, COVER_H + 54.f);
    window.draw(bioText);

    cityText.setPosition(CX + 16.f, COVER_H + 72.f);
    window.draw(cityText);

    statsText.setPosition(CX + 16.f, COVER_H + 90.f);
    window.draw(statsText);

    if (isEditingProfile && isSelf) {
        cityEditInput->draw(window);
        bioEditInput->draw(window);
        saveProfileButton->draw(window);
    } else {
        if (isSelf) editProfileButton->draw(window);
        else        followButton->draw(window);
    }

    // Tabs
    DrawUtils::drawDivider(window, CX, HEADER_H, CW);
    for (int i = 0; i < 3; ++i) {
        tabText[i].setFillColor(i == activeProfileTab ? Theme::TEXT_WHITE : Theme::TEXT_DIM);
        window.draw(tabText[i]);
    }
    tabIndicator.setPosition(tabIndX.current, TAB_Y + 28.f);
    window.draw(tabIndicator);
    DrawUtils::drawDivider(window, CX, VP_Y - 1.f, CW);

    // Posts
    sf::View orig = window.getView();
    window.setView(feedViewport);
    if (postCards.empty()) window.draw(emptyText);
    else for (const auto& card : postCards) card->draw(window);
    window.setView(orig);

    if (maxScrollOffset > 0.f) {
        window.draw(scrollTrack);
        window.draw(scrollThumb);
    }
}

void ProfileScreen::handleEvent(sf::Event& event) {
    if (!currentUser || !targetUser) return;
    sidebar.handleEvent(event);

    bool isSelf = currentUser->getUsername() == targetUser->getUsername();

    // Tab clicks
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mp(static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));
        for (int i = 0; i < 3; ++i) {
            if (tabText[i].getGlobalBounds().contains(mp) && activeProfileTab != i) {
                activeProfileTab = i;
                return;
            }
        }
    }

    if (isSelf) {
        if (isEditingProfile) {
            cityEditInput->handleEvent(event);
            bioEditInput->handleEvent(event);
            if (saveProfileButton->isClicked(event)) {
                isEditingProfile = false;
                currentUser->setCity(cityEditInput->getText());
                currentUser->setBio(bioEditInput->getText());
                userManager.saveToFile("data/users.txt");
                reloadProfile();
                ToastManager::instance().push("Profile saved", "", ToastType::SUCCESS);
                return;
            }
        } else {
            editProfileButton->handleEvent(event);
            if (editProfileButton->isClicked(event)) {
                isEditingProfile = true;
                cityEditInput->setText(currentUser->getCity());
                bioEditInput->setText(currentUser->getBio());
                return;
            }
        }
    } else {
        followButton->handleEvent(event);
        if (followButton->isClicked(event)) {
            bool following = socialGraph.isFollowing(
                currentUser->getUsername(), targetUser->getUsername());
            if (following) {
                socialGraph.unfollow(currentUser->getUsername(), targetUser->getUsername());
                targetUser->setFollowerCount(std::max(0, targetUser->getFollowerCount() - 1));
                currentUser->setFollowingCount(std::max(0, currentUser->getFollowingCount() - 1));
                ToastManager::instance().push("Unfollowed @" + targetUser->getUsername(),
                    "", ToastType::INFO);
            } else {
                socialGraph.follow(currentUser->getUsername(), targetUser->getUsername());
                targetUser->setFollowerCount(targetUser->getFollowerCount() + 1);
                currentUser->setFollowingCount(currentUser->getFollowingCount() + 1);
                ToastManager::instance().push("Following @" + targetUser->getUsername(),
                    "", ToastType::SUCCESS);
            }
            userManager.saveToFile("data/users.txt");
            socialGraph.saveToFile("data/follows.txt");
            reloadProfile();
        }
    }

    // Scroll
    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 60.f;
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

    for (auto& card : postCards) {
        card->handleEvent(me);
        if (hasMouse && card->isHandleClicked(me)) clickedHandle = card->getAuthorUsername();
        if (hasMouse && card->isCommentClicked(me)) clickedPostId = card->getPostId();
        if (hasMouse && card->isLikeClicked(me))
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
    }
}

void ProfileScreen::update(float dt) {
    sidebar.update(dt);
    followButton->update(dt);
    editProfileButton->update(dt);
    saveProfileButton->update(dt);
    if (isEditingProfile) {
        cityEditInput->update(dt);
        bioEditInput->update(dt);
    }

    tabIndX.setTarget(tabText[activeProfileTab].getPosition().x);
    tabIndX.update(dt);
    tabIndicator.setSize(sf::Vector2f(tabText[activeProfileTab].getGlobalBounds().width, 3.f));

    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else scrollOffset = targetScrollOffset;
    feedViewport.setCenter(CW / 2.f, VP_H / 2.f + scrollOffset);

    // Scrollbar thumb
    float total = maxScrollOffset + VP_H;
    if (total <= 0.f) total = VP_H;
    float thumbH = std::max(30.f, VP_H * (VP_H / total));
    scrollThumb.setSize(sf::Vector2f(3.f, thumbH));
    float ratio = maxScrollOffset > 0.f ? scrollOffset / maxScrollOffset : 0.f;
    scrollThumb.setPosition(CX + CW - 6.f, VP_Y + ratio * (VP_H - thumbH));

    for (auto& card : postCards) card->update(dt);
}

bool ProfileScreen::isBackClicked(sf::Event& e, sf::RenderWindow&) { return isHomeClicked(e); }

std::string ProfileScreen::getClickedHandle()   { return clickedHandle; }
void        ProfileScreen::clearClickedHandle() { clickedHandle = ""; }
int         ProfileScreen::getClickedPostId()   { return clickedPostId; }
void        ProfileScreen::clearClickedPostId() { clickedPostId = -1; }
