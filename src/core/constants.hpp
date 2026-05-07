#ifndef SOCCER_CONSTANTS_HPP
#define SOCCER_CONSTANTS_HPP

#include <cstdint>
#include <raylib.h>

/* === Constants =========================================================== */
// Window
constexpr int WINDOW_WIDTH = 1316;
constexpr int WINDOW_HEIGHT = 896;

// Pitch
constexpr int PITCH_PANEL_PADDING_TOP = 16;
constexpr int PITCH_PANEL_PADDING_SIDES = 16;
constexpr int PITCH_PANEL_HEIGHT = 720 - (2 * PITCH_PANEL_PADDING_TOP);
constexpr int PITCH_PANEL_WIDTH = WINDOW_WIDTH - (2 * PITCH_PANEL_PADDING_SIDES);
constexpr int PITCH_PANEL_X = 0 + PITCH_PANEL_PADDING_SIDES;
constexpr int PITCH_PANEL_Y = PITCH_PANEL_PADDING_TOP;

// Pitch Border
constexpr int PITCH_BORDER_X = PITCH_PANEL_X - PITCH_PANEL_PADDING_SIDES;
constexpr int PITCH_BORDER_Y = PITCH_PANEL_Y - PITCH_PANEL_PADDING_TOP;
constexpr int PITCH_BORDER_WIDTH = PITCH_PANEL_WIDTH + (2 * PITCH_PANEL_PADDING_SIDES);
constexpr int PITCH_BORDER_HEIGHT = PITCH_PANEL_HEIGHT + (2 * PITCH_PANEL_PADDING_TOP);

// Pitch Regions
constexpr int PITCH_REGIONS_VERTICAL = 3;
constexpr int PITCH_REGIONS_HORIZONTAL = 6;
constexpr auto TOTAL_PITCH_REGIONS = PITCH_REGIONS_HORIZONTAL * PITCH_REGIONS_VERTICAL;
static_assert(TOTAL_PITCH_REGIONS == 18, "Pitch region configuration out of band with book.");
constexpr int PITCH_REGION_WIDTH = PITCH_PANEL_WIDTH / PITCH_REGIONS_HORIZONTAL;
constexpr int PITCH_REGION_HEIGHT = PITCH_PANEL_HEIGHT / PITCH_REGIONS_VERTICAL;

// Pitch Lines
constexpr float GOAL_OFFSET = static_cast<float>(PITCH_REGION_WIDTH) / 3.0f;
constexpr float CENTER_CIRCLE_RADIUS = static_cast<float>(PITCH_REGION_WIDTH);

// BSS
constexpr uint8_t BSS_SLOTS_HORIZONTAL = 6;
constexpr uint8_t BSS_SLOTS_VERTICAL = 5;
constexpr uint8_t TOTAL_BSS_SLOTS = BSS_SLOTS_HORIZONTAL * BSS_SLOTS_VERTICAL;
static_assert(TOTAL_BSS_SLOTS == 30, "BSS slot configuration out of band with book.");

// The BSS Area appears to be the width of one half the field minus the goal offset, and this performed on 
// both sides (the goal and equivalent distance from the center). Thus, the below should create a "square radius"
// for the BSS area to follow, where the BSS area should then be targeted to the center of the half of the arena
// (depending on the team).
constexpr int BSS_AREA_SIDE = (PITCH_PANEL_WIDTH / 2) - (GOAL_OFFSET * 2);

// Player
constexpr float PLAYER_MAX_DISTANCE_PER_SECOND = 25.0f;
constexpr float PLAYER_RADIUS = 44.0f;

// Ball
constexpr float BALL_MAX_DISTANCE_PER_SECOND = 50.0f;
constexpr float BALL_MASS = 1.0f;
constexpr float BALL_FRICTION = 5.0f;
constexpr float BALL_RESET_FORCE = 50.0f;
constexpr float BALL_RADIUS = 22.0f;

// Application Title
constexpr const char* APPLICATION_TITLE = "Simple Soccer";

#endif