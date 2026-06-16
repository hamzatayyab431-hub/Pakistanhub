#pragma once
#include <SFML/Graphics.hpp>
#include "Theme.h"
#include "Animator.h"
#include "UserManager.h"
#include "SocialGraph.h"
#include <string>
#include <vector>

struct SuggestedUser {
    std::string displayName;
    std::string username;
    char        initial;
    Lerp        hoverAlpha;
    bool        isHovered = false;
};

class RightPanel {
public:
    RightPanel() = default;
    void init(sf::Font& font, UserManager& um, SocialGraph& sg);
    void setCurrentUser(const std::string& username);

    void draw(sf::RenderWindow& window);
    void handleEvent(sf::Event& event);
    void update(float dt);

    std::string getClickedHandle();
    void        clearClickedHandle();

private:
    sf::Font*      font       = nullptr;
    UserManager*   userMgr    = nullptr;
    SocialGraph*   socialGraph= nullptr;
    std::string    currentUsername;
    std::string    clickedHandle;

    sf::Text trendingTitle;
    sf::Text whoToFollowTitle;

    struct TrendItem { std::string tag; std::string count; };
    std::vector<TrendItem> trends;
    std::vector<SuggestedUser> suggestions;

    void buildSuggestions();
    void drawSectionTitle(sf::RenderWindow& w, sf::Text& t);
};
