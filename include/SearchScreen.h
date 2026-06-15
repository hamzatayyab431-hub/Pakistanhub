#ifndef SEARCHSCREEN_H
#define SEARCHSCREEN_H

#include "UIComponent.h"
#include "TextInput.h"
#include "GlassButton.h"
#include "UserManager.h"
#include "PostManager.h"
#include "SocialGraph.h"
#include <memory>
#include <vector>

// Helper card widget for displaying a user search result
class SearchResultCard : public UIComponent {
private:
    User* user;
    sf::Font& font;
    sf::Vector2f position;
    sf::Vector2f size;
    bool isHovered;

    sf::RectangleShape backgroundRect;
    sf::RectangleShape accentBar;
    sf::CircleShape avatarCircle;
    sf::Text avatarLetter;
    sf::Text displayNameText;
    sf::Text usernameText;
    sf::Text statsText;

public:
    SearchResultCard(User* usr, sf::Font& fnt, const sf::Vector2f& pos, const sf::Vector2f& sz);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    bool isClicked(sf::Event& event);
    std::string getUsername() const;
    float getHeight() const { return size.y; }
};

class SearchScreen : public UIComponent {
private:
    sf::Font& font;
    UserManager& userManager;
    PostManager& postManager;
    SocialGraph& socialGraph;
    User* currentUser;

    // Persistent Top Navigation Bar
    sf::RectangleShape headerBackground;
    sf::Text logoText;
    sf::Text statusText;
    std::unique_ptr<GlassButton> navHome;
    std::unique_ptr<GlassButton> navSearch;
    std::unique_ptr<GlassButton> navProfile;
    std::unique_ptr<GlassButton> navLogout;
    sf::RectangleShape navAccentLine;

    // Search input
    std::unique_ptr<TextInput> searchInput;
    std::string currentQuery;

    // Scrollable results viewport
    sf::View resultsViewport;
    float scrollOffset;
    float targetScrollOffset;
    float maxScrollOffset;

    std::vector<std::unique_ptr<SearchResultCard>> resultCards;
    std::string clickedHandle;

    void updateResults();

public:
    SearchScreen(sf::Font& fnt, UserManager& um, PostManager& pm, SocialGraph& sg);

    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event& event) override;
    void update() override;

    void setCurrentUser(User* user);
    void reloadSearch();
    std::string getClickedHandle();
    void clearClickedHandle();

    bool isHomeClicked(sf::Event& event);
    bool isSearchClicked(sf::Event& event);
    bool isProfileClicked(sf::Event& event);
    bool isLogoutClicked(sf::Event& event);
};

#endif // SEARCHSCREEN_H
