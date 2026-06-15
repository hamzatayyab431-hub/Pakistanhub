#include "GlassPanel.h"
#include "DrawUtils.h"
#include "Theme.h"

void GlassPanel::draw(sf::RenderWindow& window,
                      const sf::FloatRect& bounds,
                      bool glowing,
                      float /*outlineThickness*/,
                      sf::Color /*outlineColor*/)
{
    sf::Color fill = glowing ? Theme::GLASS_2 : Theme::GLASS_1;
    DrawUtils::drawGlassPanel(window, bounds, 6.f, fill, glowing, Theme::GREEN_GLOW);
}
