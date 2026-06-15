#include "PostDetailScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include "ToastManager.h"
#include <algorithm>

PostDetailScreen::PostDetailScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                                   CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg),
      currentUser(nullptr), targetPostLiked(false),
      scrollOffset(0.f), targetScrollOffset(0.f), maxScrollOffset(0.f)
{
    nav.init(font, NavActive::NONE, "");

    // Reply area
    replyBackground.setPosition(100.f, 630.f);
    replyBackground.setSize(sf::Vector2f(1080.f, 72.f));

    replyInput = std::make_unique<TextInput>(
        sf::Vector2f(115.f, 645.f), sf::Vector2f(800.f, 44.f),
        font, "Write a reply...");

    replyButton = std::make_unique<GlassButton>(
        sf::Vector2f(930.f, 645.f), sf::Vector2f(220.f, 44.f),
        font, "Reply");

    // Comments viewport: y[258,622], height=364
    commentViewport.setSize(1080.f, 364.f);
    commentViewport.setViewport(sf::FloatRect(100.f / 1280.f, 258.f / 720.f,
                                              1080.f / 1280.f, 364.f / 720.f));
    commentViewport.setCenter(540.f, 182.f);
}

void PostDetailScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser) nav.setUsername(currentUser->getUsername());
}

void PostDetailScreen::setTargetPost(const Post& post, bool liked) {
    targetPost = post;
    targetPostLiked = liked;
    int cc = commentManager.getCommentCountForPost(targetPost.getPostId());
    postCard = std::make_unique<PostCard>(
        targetPost, font, sf::Vector2f(100.f, 66.f),
        sf::Vector2f(1080.f, 130.f), targetPostLiked, cc);
    reloadComments();
}

void PostDetailScreen::reloadComments() {
    commentCards.clear();
    scrollOffset = 0.f;
    targetScrollOffset = 0.f;
    commentViewport.setCenter(540.f, 182.f);

    auto thread = commentManager.getCommentsForPost(targetPost.getPostId());
    float y = 10.f;
    for (const auto& cmt : thread) {
        auto card = std::make_unique<CommentCard>(
            cmt, font, sf::Vector2f(0.f, y), sf::Vector2f(1080.f, 80.f));
        y += card->getHeight() + 10.f;
        commentCards.push_back(std::move(card));
    }
    maxScrollOffset = std::max(0.f, y - 364.f);
}

void PostDetailScreen::draw(sf::RenderWindow& window) {
    nav.draw(window);

    if (postCard) postCard->draw(window);

    // Separator line
    sf::RectangleShape sep(sf::Vector2f(1080.f, 1.f));
    sep.setPosition(100.f, 250.f);
    sep.setFillColor(Theme::GLASS_BORDER);
    window.draw(sep);

    // Reply bar
    DrawUtils::drawGlassPanel(window, replyBackground.getGlobalBounds(),
        6.f, Theme::GLASS_1);
    replyInput->draw(window);
    replyButton->draw(window);

    // Comments
    sf::View orig = window.getView();
    window.setView(commentViewport);
    for (const auto& card : commentCards) card->draw(window);
    window.setView(orig);
}

void PostDetailScreen::handleEvent(sf::Event& event) {
    nav.handleEvent(event);
    replyInput->handleEvent(event);
    replyButton->handleEvent(event);

    if (postCard) {
        postCard->handleEvent(event);
        if (postCard->isLikeClicked(event)) {
            postManager.toggleLike(postCard->getPostId(), postCard->getIsLiked());
            targetPostLiked = postCard->getIsLiked();
            if (targetPostLiked) {
                ToastManager::instance().push("Liked", "", ToastType::INFO);
            }
        }
    }

    if (currentUser && replyButton->isClicked(event)) {
        std::string txt = replyInput->getText();
        bool hasContent = false;
        for (char c : txt) if (!std::isspace(static_cast<unsigned char>(c))) {
            hasContent = true; break; }
        if (hasContent) {
            commentManager.addComment(targetPost.getPostId(),
                currentUser->getUsername(), txt);
            commentManager.saveToFile("data/comments.txt");
            replyInput->clear();
            reloadComments();
            int cc = commentManager.getCommentCountForPost(targetPost.getPostId());
            postCard = std::make_unique<PostCard>(
                targetPost, font, sf::Vector2f(100.f, 66.f),
                sf::Vector2f(1080.f, 130.f), targetPostLiked, cc);
            ToastManager::instance().push("Comment added", "", ToastType::SUCCESS);
        }
    }

    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
        targetScrollOffset -= event.mouseWheelScroll.delta * 50.f;
        if (targetScrollOffset < 0.f) targetScrollOffset = 0.f;
        if (targetScrollOffset > maxScrollOffset) targetScrollOffset = maxScrollOffset;
    }

    sf::Event me = event;
    if (event.type == sf::Event::MouseButtonPressed ||
        event.type == sf::Event::MouseButtonReleased) {
        me.mouseButton.x = static_cast<int>(event.mouseButton.x - 100.f);
        me.mouseButton.y = static_cast<int>((event.mouseButton.y - 258.f) + scrollOffset);
    } else if (event.type == sf::Event::MouseMoved) {
        me.mouseMove.x = static_cast<int>(event.mouseMove.x - 100.f);
        me.mouseMove.y = static_cast<int>((event.mouseMove.y - 258.f) + scrollOffset);
    }
    for (auto& card : commentCards) card->handleEvent(me);
}

void PostDetailScreen::update(float dt) {
    nav.update(dt);
    replyInput->update(dt);
    replyButton->update(dt);

    if (postCard) postCard->update(dt);

    if (replyInput) {
        std::string txt = replyInput->getText();
        bool has = false;
        for (char c : txt) if (!std::isspace(static_cast<unsigned char>(c))) { has = true; break; }
        replyButton->setEnabled(has);
    }

    float lerpF = std::min(1.f, 12.f * dt);
    if (std::abs(targetScrollOffset - scrollOffset) > 0.05f)
        scrollOffset += (targetScrollOffset - scrollOffset) * lerpF;
    else
        scrollOffset = targetScrollOffset;
    commentViewport.setCenter(540.f, 182.f + scrollOffset);
}

void PostDetailScreen::update() { update(0.016f); }

bool PostDetailScreen::isHomeClicked(sf::Event& e)    { return nav.homeClicked(e); }
bool PostDetailScreen::isSearchClicked(sf::Event& e)  { return nav.searchClicked(e); }
bool PostDetailScreen::isProfileClicked(sf::Event& e) { return nav.profileClicked(e); }
bool PostDetailScreen::isLogoutClicked(sf::Event& e)  { return nav.logoutClicked(e); }
