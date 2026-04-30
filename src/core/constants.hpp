#ifndef SOCCER_CONSTANTS_HPP
#define SOCCER_CONSTANTS_HPP

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

// Application Title
constexpr const char* APPLICATION_TITLE = "Simple Soccer";

#endif