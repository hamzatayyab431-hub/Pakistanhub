#pragma once
#include <SFML/Graphics.hpp>

namespace Theme {
    // ── Backgrounds ──────────────────────────────────────────────────────────
    inline const sf::Color BG_PRIMARY    = {11,  17,  32,  255};  // #0B1120
    inline const sf::Color BG_SECONDARY  = {17,  24,  39,  255};  // #111827
    inline const sf::Color BG_SURFACE    = {26,  35,  51,  255};  // #1A2333
    inline const sf::Color BG_ELEVATED   = {30,  42,  62,  255};  // card hover

    // Legacy aliases so untouched files compile
    inline const sf::Color BG_BASE       = BG_PRIMARY;
    inline const sf::Color BG_DEEP       = BG_PRIMARY;
    inline const sf::Color BG_DARK       = BG_PRIMARY;
    inline const sf::Color BG_MID        = BG_SECONDARY;

    // ── Pakistan Green family ─────────────────────────────────────────────────
    inline const sf::Color GREEN         = {0,   210, 106, 255};  // #00D26A primary
    inline const sf::Color GREEN_DARK    = {0,   184, 92,  255};  // #00B85C secondary
    inline const sf::Color GREEN_MINT    = {46,  255, 155, 255};  // #2EFF9B accent
    inline const sf::Color GREEN_DIM     = {0,   210, 106, 120};  // semi-transparent
    inline const sf::Color GREEN_SUBTLE  = {0,   210, 106, 28};   // very faint bg
    inline const sf::Color GREEN_GLOW    = {0,   210, 106, 18};   // ambient glow
    inline const sf::Color GREEN_GLOW2   = {0,   210, 106, 8};    // far glow

    // Legacy aliases
    inline const sf::Color GREEN_PRIMARY = GREEN;

    // ── Sidebar / panel fills ─────────────────────────────────────────────────
    inline const sf::Color SIDEBAR_BG    = {15,  22,  36,  255};  // slightly distinct
    inline const sf::Color CARD_BG       = {20,  29,  45,  255};  // post card base
    inline const sf::Color CARD_HOVER    = {25,  36,  55,  255};  // post card hover
    inline const sf::Color INPUT_BG      = {14,  20,  33,  255};  // input field fill
    inline const sf::Color DIVIDER       = {255, 255, 255, 18};   // subtle separator

    // ── Glass layers (kept for compatibility) ─────────────────────────────────
    inline const sf::Color GLASS_1       = {255, 255, 255, 10};
    inline const sf::Color GLASS_2       = {255, 255, 255, 18};
    inline const sf::Color GLASS_3       = {255, 255, 255, 28};
    inline const sf::Color GLASS_BORDER  = {255, 255, 255, 22};
    inline const sf::Color GLASS_TOP     = {255, 255, 255, 40};
    inline const sf::Color GLASS_FILL    = GLASS_1;

    // ── Text ─────────────────────────────────────────────────────────────────
    inline const sf::Color TEXT_WHITE    = {255, 255, 255, 255};  // primary text
    inline const sf::Color TEXT_MUTED    = {148, 163, 184, 255};  // #94A3B8 secondary
    inline const sf::Color TEXT_DIM      = {100, 116, 139, 255};  // #64748B muted
    inline const sf::Color TEXT_GREEN    = GREEN;

    // Legacy aliases
    inline const sf::Color TEXT_PRIMARY  = TEXT_WHITE;

    // ── Semantic ─────────────────────────────────────────────────────────────
    inline const sf::Color ERROR         = {239, 68,  68,  255};  // #EF4444
    inline const sf::Color WARNING       = {245, 158, 11,  255};  // #F59E0B
    inline const sf::Color SUCCESS       = {34,  197, 94,  255};  // #22C55E
    inline const sf::Color INFO          = {59,  130, 246, 255};  // #3B82F6

    // Legacy aliases
    inline const sf::Color ACCENT_RED    = ERROR;

    // ── Layout constants ─────────────────────────────────────────────────────
    inline constexpr float SIDEBAR_W     = 220.f;
    inline constexpr float RIGHT_W       = 290.f;
    inline constexpr float CENTER_X      = SIDEBAR_W;
    inline constexpr float CENTER_W      = 1280.f - SIDEBAR_W - RIGHT_W;   // 770px
    inline constexpr float RIGHT_X       = 1280.f - RIGHT_W;

    // ── Overlays ─────────────────────────────────────────────────────────────
    inline const sf::Color OVERLAY_DARK  = {0,   0,   0,   160};
    inline const sf::Color SCRIM         = {0,   0,   0,   200};
}
