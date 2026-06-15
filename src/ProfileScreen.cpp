#include "ProfileScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include "ToastManager.h"
#include <algorithm>
#include <iostream>
#include <cctype>

ProfileScreen::ProfileScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                             CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg),
      currentUser(nullptr), targetUser(nullptr),
      isEditingProfile(false),
      scrollOffset(0.f), targetScrollOffset(0.f), maxScrollOffset(0.f),
      clickedHandle(""), clickedPostId(-1)
{
    nav.init(font, NavActive::PROFILE, "");

    // Profile header card: y=68, height=130 (expands to 170 in edit mode — handled in draw)
    headerCard.setPosition(100.f, 68.f);
    headerCard.setSize(sf::Vector2f(1080.f, 130.f));

    // Text elements
    auto makeText = [&](sf::Text& t, unsigned int sz, sf::Color col) {
        t.setFont(font);
        t.setCharacterSize(sz);
        t.setFillColor(col);
    };
    makeText(nameText,   22, Theme::TEXT_WHITE);  nameText.setStyle(sf::Text::Bold);
    makeText(handleText, 14, Theme::TEXT_GREEN);
    makeText(cityText,   13, Theme::TEXT_MUTED);
    makeText(bioText,    13, Theme::TEXT_MUTED);
    makeText(statsText,  14, Theme::TEXT_WHITE);

    nameText.setPosition(195.f, 76.f);
    handleText.setPosition(195.f, 102.f);
    cityText.setPosition(195.f, 122.f);
    bioText.setPosition(195.f, 141.f);
    statsText.setPosition(195.f, 163.f);

    // Follow / edit buttons
    followButton = std::make_unique<GlassButton>(
        sf::Vector2f(940.f, 110.f), sf::Vector2f(200.f, 40.f), font, "Follow");
    editProfileButton = std::make_unique<GlassButton>(
        sf::Vector2f(940.f, 110.f), sf::Vector2f(200.f, 40.f),
        font, "Edit Profile", GlassButton::Type::NAV_DEFAULT);
    saveProfileButton = std::make_unique<GlassButton>(
        sf::Vector2f(940.f, 110.f), sf::Vector2f(200.f, 40.f), font, "Save Profile");

    cityEditInput  = std::make_unique<TextInput>(
        sf::Vector2f(195.f, 120.f), sf::Vector2f(300.f, 26.f), font, "City...");
    bioEditInput   = std::make_unique<TextInput>(
        sf::Vector2f(195.f, 150.f), sf::Vector2f(500.f, 26.f), font, "Bio...");

    // Empty state
    emptyText.setFont(font); emptyText.setCharacterSize(15);
    emptyText.setFillColor(Theme::TEXT_MUTED);
    emptyText.setString("No posts yet.");
    sf::FloatRect eb = emptyText.getLocalBounds();
    emptyText.setOrigin(eb.left + eb.width / 2.f, eb.top + eb.height / 2.f);
    emptyText.setPosition(540.f, 100.f);

    // Scrollbar
    scrollTrack.setSize(sf::Vector2f(4.f, 460.f));
    scrollTrack.setPosition(1278.f, 210.f);
    scrollTrack.setFillColor(Theme::GLASS_BORDER);
    scrollThumb.setSize(sf::Vector2f(4.f, 60.f));
    scrollThumb.setPosition(1278.f, 210.f);
    scrollThumb.setFillColor(Theme::GREEN_DIM);

    // Viewport: x[100,1180], y[210,670]
    feedViewport.setSize(1080.f, 460.f);
    feedViewport.setViewport(sf::FloatRect(100.f / 1280.f, 210.f / 720.f,
                                           1080.f / 1280.f, 460.f / 720.f));
    feedViewport.setCenter(540.f, 230.f);
}

void ProfileScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser) nav.setUsername(currentUser->getUsername());
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
    feedViewport.setCenter(540.f, 230.f);

    if (!targetUser) return;

    nameText.setString(targetUser->getDisplayName());
    handleText.setString("@" + targetUser->getUsername());

    std::string city = targetUser->getCity();
    cityText.setString(city.empty() ? "" : city);

    std::string bio = targetUser->getBio();
    bioText.setString(bio.empty() ? "" : bio);

    int friends = static_cast<int>(socialGraph.getFriends(targetUser->getUsername()).size());
    statsText.setString(std::to_string(targetUser->getFollowerCount()) + " Followers  ·  "
        + std::to_string(targetUser->getFollowingCount()) + " Following  ·  "
        + std::to_string(friends) + " Friends");

    // Configure follow/edit button
    bool isSelf = currentUser && targetUser &&
                  currentUser->getUsername() == targetUser->getUsername();
    if (isSelf) {
        followButton->setPosition(sf::Vector2f(3000.f, 3000.f));
        editProfileButton->setPosition(sf::Vector2f(940.f, 110.f));
        saveProfileButton->setPosition(sf::Vector2f(3000.f, 3000.f));
    } else {
        editProfileButton->setPosition(sf::Vector2f(3000.f, 3000.f));
        saveProfileButton->setPosition(sf::Vector2f(3000.f, 3000.f));
        bool following = currentUser &&
            socialGraph.isFollowing(currentUser->getUsername(), targetUser->getUsername());
        followButton->setPosition(sf::Vector2f(940.f, 110.f));
        followButton = std::make_unique<GlassButton>(
            sf::Vector2f(940.f, 110.f), sf::Vector2f(200.f, 40.f), font,
            following ? "Unfollow" : "Follow",
            following ? GlassButton::Type::NAV_DEFAULT : GlassButton::Type::PRIMARY);
    }

    // Load posts
    std::vector<Post> userPosts = postManager.getPostsByUser(targetUser->getUsername());
    std::sort(userPosts.begin(), userPosts.end(), [](const Post& a, const Post& b) {
        return a.getTimestamp() != b.getTimestamp()
            ? a.getTimestamp() > b.getTimestamp()
            : a.getPostId() > b.getPostId();
    });

    float y = 10.f;
    for (const auto& post : userPosts) {
        int cc = commentManager.getCommentCountForPost(post.getPostId());
        auto card = std::make_unique<PostCard>(
            post, font, sf::Vector2f(0.f, y), sf::Vector2f(1080.f, 120.f), false, cc);
        y += card->getHeight() + 12.f;
        postCards.push_back(std::move(card));
    }
    maxScrollOffset = std::max(0.f, y - 460.f);
}

void ProfileScreen::draw(sf::RenderWindow& window) {
    nav.draw(window);

    // Header card grows taller in edit mode to avoid overlaps
    bool isSelf = currentUser && targetUser &&
                  currentUser->getUsername() == targetUser->getUsername();
    float cardH = (isEditingProfile && isSelf) ? 180.f : 130.f;
    headerCard.setSize(sf::Vector2f(1080.f, cardH));

    // Profile header card (GLASS_3 — more opaque)
    DrawUtils::drawGlassPanel(window, headerCard.getGlobalBounds(), 8.f, Theme::GLASS_3);

    // Avatar (70px diameter = radius 35), centred vertically in card
    float avatarCY = 68.f + cardH / 2.f;
    if (targetUser && !targetUser->getDisplayName().empty()) {
        DrawUtils::drawAvatar(window, sf::Vector2f(148.f, avatarCY),
            35.f, targetUser->getDisplayName()[0], font, 28);
    }

    window.draw(nameText);
    window.draw(handleText);

    if (isEditingProfile && isSelf) {
        // Edit mode: City input at row 1, Bio input at row 2, Save button alongside
        cityEditInput->draw(window);
        bioEditInput->draw(window);
        saveProfileButton->draw(window);
    } else {
        window.draw(cityText);
        window.draw(bioText);
        window.draw(statsText);

        if (isSelf) {
            editProfileButton->draw(window);
        } else {
            followButton->draw(window);
        }
    }

    // Scrollable posts — shift viewport down when card is taller
    float vpTop = 68.f + cardH + 8.f;           // 8px gap below header card
    float vpH   = 720.f - vpTop - 8.f;          // fill to near bottom
    if (vpH < 100.f) vpH = 100.f;
    feedViewport.setSize(sf::Vector2f(1080.f, vpH));
    feedViewport.setViewport(sf::FloatRect(100.f / 1280.f, vpTop / 720.f,
                                           1080.f / 1280.f, vpH / 720.f));

    // Keep scrollbar track aligned with viewport
    scrollTrack.setPosition(1278.f, vpTop);
    scrollTrack.setSize(sf::Vector2f(4.f, vpH));

    sf::View orig = window.getView();
    window.setView(feedViewport);
    if (postCards.empty()) {
        window.draw(emptyText);
    } else {
        for (const auto& card : postCards) card->draw(window);
    }
    window.setView(orig);

    if (maxScrollOffset > 0.f) {
        window.draw(scrollTrack);
        window.draw(scrollThumb);
    }
}

void ProfileScreen::handleEvent(sf::Event& event) {
    if (!currentUser || !targetUser) return;

    nav.handleEvent(event);

    bool isSelf = currentUser->getUsername() == targetUser->getUsername();
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
                // Reposition inputs for the expanded card (card top=68, height=180)
                cityEditInput  = std::make_unique<TextInput>(
                    sf::Vector2f(195.f, 122.f), sf::Vector2f(440.f, 30.f), font, "City...");
                bioEditInput   = std::make_unique<TextInput>(
                    sf::Vector2f(195.f, 160.f), sf::Vector2f(440.f, 30.f), font, "Bio...");
                saveProfileButton = std::make_unique<GlassButton>(
                    sf::Vector2f(650.f, 122.f), sf::Vector2f(160.f, 68.f), font, "Save");
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
        me.mouseButton.x = static_cast<int>(event.mouseButton.x - 100.f);
        me.mouseButton.y = static_cast<int>((event.mouseButton.y - 210.f) + scrollOffset);
        hasMouse = true;
    } else if (event.type == sf::Event::MouseMoved) {
        me.mouseMove.x = static_cast<int>(event.mouseMove.x - 100.f);
        me.mouseMove.y = static_cast<int>((event.mouseMove.y - 210.f) + scrollOffset);
        hasMouse = true;
    }

    for (auto& card : postCards) {
        card->handleEvent(me);
        if (hasMouse && card->isHandleClicked(me)) clickedHandle = card->getAuthorUsername();
        if (hasMouse && card->isCommentClicked(me)) clickedPostId = card->getPostId();
        if (hasMouse && card->isLikeClicked(me)) {
            postManager.toggleLike(card->getPostId(), card->getIsLiked());
        }
    }
}

void ProfileScreen::update(float dt) {
    nav.update(dt);
    followButton->update(dt);
    editProfileButton->update(dt);
    saveProfileButton->update(dt);
    if (isEditingProfile) {
        cityEditInput->update(dt);
        bioEditInput->update(dt);
    }

    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else
        scrollOffset = targetScrollOffset;

    // Card height depends on edit mode
    bool isSelf = currentUser && targetUser &&
                  currentUser->getUsername() == targetUser->getUsername();
    float cardH = (isEditingProfile && isSelf) ? 180.f : 130.f;
    float vpTop = 68.f + cardH + 8.f;
    float vpH   = 720.f - vpTop - 8.f;
    if (vpH < 100.f) vpH = 100.f;

    feedViewport.setCenter(540.f, vpH / 2.f + scrollOffset);

    // Scrollbar thumb
    float total = maxScrollOffset + vpH;
    if (total <= 0.f) total = vpH;
    float thumbH = std::max(30.f, vpH * (vpH / total));
    scrollThumb.setSize(sf::Vector2f(4.f, thumbH));
    float scrollRatio = maxScrollOffset > 0.f ? scrollOffset / maxScrollOffset : 0.f;
    scrollThumb.setPosition(1278.f, vpTop + scrollRatio * (vpH - thumbH));

    for (auto& card : postCards) card->update(dt);
}

void ProfileScreen::update() { update(0.016f); }

bool ProfileScreen::isBackClicked(sf::Event& e, sf::RenderWindow&) { return isHomeClicked(e); }
bool ProfileScreen::isHomeClicked(sf::Event& e)    { return nav.homeClicked(e); }
bool ProfileScreen::isSearchClicked(sf::Event& e)  { return nav.searchClicked(e); }
bool ProfileScreen::isProfileClicked(sf::Event& e) { return nav.profileClicked(e); }
bool ProfileScreen::isLogoutClicked(sf::Event& e)  { return nav.logoutClicked(e); }

std::string ProfileScreen::getClickedHandle()   { return clickedHandle; }
void        ProfileScreen::clearClickedHandle() { clickedHandle = ""; }
int         ProfileScreen::getClickedPostId()   { return clickedPostId; }
void        ProfileScreen::clearClickedPostId() { clickedPostId = -1; }
