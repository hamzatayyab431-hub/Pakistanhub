#include "PostDetailScreen.h"
#include "DrawUtils.h"
#include "Theme.h"
#include "ToastManager.h"
#include <algorithm>
#include <cctype>

static constexpr float POST_H_APPROX = 150.f;
static constexpr float REPLY_H = 60.f;
static constexpr float SEP_Y   = POST_H_APPROX + 8.f;
static constexpr float CVP_Y   = SEP_Y + 10.f;
static constexpr float CVP_H_BASE = 720.f - CVP_Y - REPLY_H - 16.f;

PostDetailScreen::PostDetailScreen(sf::Font& fnt, UserManager& um, PostManager& pm,
                                   CommentManager& cm, SocialGraph& sg)
    : font(fnt), userManager(um), postManager(pm), commentManager(cm), socialGraph(sg)
{
    sidebar.init(font, SidebarItem::NONE, "", "");

    replyBackground.setPosition(CX, 720.f - REPLY_H - 8.f);
    replyBackground.setSize(sf::Vector2f(CW, REPLY_H));

    replyInput = std::make_unique<TextInput>(
        sf::Vector2f(CX + 8.f, 720.f - REPLY_H), sf::Vector2f(CW - 130.f, 42.f),
        font, "Write a reply...");

    replyButton = std::make_unique<GlassButton>(
        sf::Vector2f(CX + CW - 118.f, 720.f - REPLY_H + 4.f),
        sf::Vector2f(108.f, 34.f), font, "Reply");

    commentViewport.setSize(sf::Vector2f(CW, CVP_H_BASE));
    commentViewport.setViewport(sf::FloatRect(CX / 1280.f, CVP_Y / 720.f,
                                              CW / 1280.f, CVP_H_BASE / 720.f));
    commentViewport.setCenter(CW / 2.f, CVP_H_BASE / 2.f);
}

void PostDetailScreen::setCurrentUser(User* user) {
    currentUser = user;
    if (currentUser)
        sidebar.setUsername(currentUser->getUsername(), currentUser->getDisplayName());
}

void PostDetailScreen::setTargetPost(const Post& post, bool liked) {
    targetPost = post;
    targetPostLiked = liked;
    int cc = commentManager.getCommentCountForPost(targetPost.getPostId());
    postCard = std::make_unique<PostCard>(
        targetPost, font, sf::Vector2f(CX, 0.f),
        sf::Vector2f(CW, 140.f), targetPostLiked, cc);
    reloadComments();
}

void PostDetailScreen::reloadComments() {
    commentCards.clear();
    scrollOffset = 0.f;
    targetScrollOffset = 0.f;
    commentViewport.setCenter(CW / 2.f, CVP_H_BASE / 2.f);

    auto thread = commentManager.getCommentsForPost(targetPost.getPostId());
    float y = 0.f;
    for (const auto& cmt : thread) {
        auto card = std::make_unique<CommentCard>(
            cmt, font, sf::Vector2f(0.f, y), sf::Vector2f(CW, 80.f));
        y += card->getHeight() + 1.f;
        commentCards.push_back(std::move(card));
    }
    maxScrollOffset = std::max(0.f, y - CVP_H_BASE);
}

void PostDetailScreen::draw(sf::RenderWindow& window) {
    sidebar.draw(window);

    sf::RectangleShape centerBg(sf::Vector2f(CW, 720.f));
    centerBg.setPosition(CX, 0.f);
    centerBg.setFillColor(Theme::BG_PRIMARY);
    window.draw(centerBg);

    if (postCard) postCard->draw(window);

    DrawUtils::drawDivider(window, CX, SEP_Y, CW);

    // Reply bar
    DrawUtils::drawCard(window,
        replyBackground.getGlobalBounds(), 0.f, Theme::CARD_BG, Theme::DIVIDER, 1.f);
    replyInput->draw(window);
    replyButton->draw(window);

    sf::View orig = window.getView();
    window.setView(commentViewport);
    for (const auto& card : commentCards) card->draw(window);
    window.setView(orig);
}

void PostDetailScreen::handleEvent(sf::Event& event) {
    sidebar.handleEvent(event);
    replyInput->handleEvent(event);
    replyButton->handleEvent(event);

    if (postCard) {
        postCard->handleEvent(event);
        if (postCard->isLikeClicked(event)) {
            postManager.toggleLike(postCard->getPostId(), postCard->getIsLiked());
            targetPostLiked = postCard->getIsLiked();
            if (targetPostLiked)
                ToastManager::instance().push("Liked", "", ToastType::INFO);
        }
    }

    if (currentUser && replyButton->isClicked(event)) {
        std::string txt = replyInput->getText();
        bool has = false;
        for (char c : txt) if (!std::isspace(static_cast<unsigned char>(c))) { has = true; break; }
        if (has) {
            commentManager.addComment(targetPost.getPostId(), currentUser->getUsername(), txt);
            commentManager.saveToFile("data/comments.txt");
            replyInput->clear();
            reloadComments();
            int cc = commentManager.getCommentCountForPost(targetPost.getPostId());
            postCard = std::make_unique<PostCard>(
                targetPost, font, sf::Vector2f(CX, 0.f),
                sf::Vector2f(CW, 140.f), targetPostLiked, cc);
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
        me.mouseButton.x = static_cast<int>(event.mouseButton.x - CX);
        me.mouseButton.y = static_cast<int>((event.mouseButton.y - CVP_Y) + scrollOffset);
    } else if (event.type == sf::Event::MouseMoved) {
        me.mouseMove.x = static_cast<int>(event.mouseMove.x - CX);
        me.mouseMove.y = static_cast<int>((event.mouseMove.y - CVP_Y) + scrollOffset);
    }
    for (auto& card : commentCards) card->handleEvent(me);
}

void PostDetailScreen::update(float dt) {
    sidebar.update(dt);
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
    else scrollOffset = targetScrollOffset;
    commentViewport.setCenter(CW / 2.f, CVP_H_BASE / 2.f + scrollOffset);
}
