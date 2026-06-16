#ifndef SEARCHSCREEN_H
#define SEARCHSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "UserManager.h"
#include "PostManager.h"
#include "SocialGraph.h"
#include "Sidebar.h"
#include "Animator.h"
#include <memory>
#include <vector>

class SearchResultCard : public UIComponent {
private:
    User*        user;
    sf::Font&    font;
    sf::Vector2f position;
    sf::Vector2f size;
    bool         isHovered = false;

    sf::RectangleShape backgroundRect;
    sf::Vector2f       avatarCenter;
    sf::Text           displayNameText;
    sf::Text           usernameText;
    sf::Text           statsText;
    Lerp               hoverFill;

public:
    SearchResultCard(User* usr, sf::Font& fnt,
                     const sf::Vector2f& pos, const sf::Vector2f& sz);
    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update(float dt = 0.016f);
    bool isClicked(sf::Event& event);
    std::string getUsername() const;
    float getHeight() const { return size.y; }
};

class SearchScreen : public UIComponent {
private:
    sf::Font&    font;
    UserManager& userManager;
    PostManager& postManager;
    SocialGraph& socialGraph;
    User*        currentUser = nullptr;

    Sidebar sidebar;

    std::unique_ptr<TextInput> searchInput;
    std::string currentQuery;
    sf::Text    noResultText;
    sf::Text    exploreTitle;

    sf::View    resultsViewport;
    float       scrollOffset      = 0.f;
    float       targetScrollOffset = 0.f;
    float       maxScrollOffset   = 0.f;

    std::vector<std::unique_ptr<SearchResultCard>> resultCards;
    std::string clickedHandle;

    static constexpr float CX   = Theme::SIDEBAR_W;
    static constexpr float CW   = Theme::CENTER_W;
    static constexpr float VP_Y = 108.f;
    static constexpr float VP_H = 720.f - VP_Y;

    void updateResults();

public:
    SearchScreen(sf::Font& fnt, UserManager& um, PostManager& pm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override { update(0.016f); }
    void update(float dt);

    void setCurrentUser(User* user);
    void reloadSearch();
    std::string getClickedHandle();
    void        clearClickedHandle();

    bool isHomeClicked(sf::Event& e)    { return sidebar.isClicked(e, SidebarItem::HOME); }
    bool isSearchClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::EXPLORE); }
    bool isProfileClicked(sf::Event& e) { return sidebar.isClicked(e, SidebarItem::PROFILE); }
    bool isLogoutClicked(sf::Event& e)  { return sidebar.isClicked(e, SidebarItem::LOGOUT); }
};

#endif // SEARCHSCREEN_H
