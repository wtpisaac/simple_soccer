#ifndef SOCCER_LOGIC_BSS_HPP
#define SOCCER_LOGIC_BSS_HPP

#include "../core/constants.hpp"
#include "../core/math.hpp"
#include <array>
#include <cassert>
#include <raylib.h>
#include <raymath.h>

constexpr int BSS_AREA_RADIUS = BSS_AREA_SIDE / 2;

constexpr Vector2
BSS_POS(
    Vector2 center,
    uint8_t i
) {
    constexpr float BSS_DIST_HORIZONTAL
        = static_cast<float>(BSS_AREA_SIDE) / static_cast<float>(BSS_SLOTS_HORIZONTAL);
    constexpr float BSS_DIST_VERTICAL
        = static_cast<float>(BSS_AREA_SIDE) / static_cast<float>(BSS_SLOTS_VERTICAL);

    assert(i < BSS_SLOTS_HORIZONTAL * BSS_SLOTS_VERTICAL && "BSS slot index out of bounds.");

    auto start = Vector2 {
        .x = center.x - static_cast<float>(BSS_AREA_RADIUS),
        .y = center.y - static_cast<float>(BSS_AREA_RADIUS)
    };
    return Vector2 {
        start.x + ((i % BSS_SLOTS_HORIZONTAL) * BSS_DIST_HORIZONTAL),
        // NOLINTNEXTLINE(bugprone-integer-division) -- row index from linear slot; intentional truncation
        start.y + ((i / BSS_SLOTS_HORIZONTAL) * BSS_DIST_VERTICAL)
    };
}

constexpr std::array<Vector2, BSS_SLOTS_HORIZONTAL * BSS_SLOTS_VERTICAL>
BSS_POS_LIST(
    Vector2 center
) {
    constexpr auto count = BSS_SLOTS_HORIZONTAL * BSS_SLOTS_VERTICAL;
    std::array<Vector2, count> pos;
    for (auto i = 0; i < count; i++) {
        pos[i] = BSS_POS(center, i);
    }
    return pos;
}

constexpr std::array<Vector2, BSS_SLOTS_HORIZONTAL * BSS_SLOTS_VERTICAL>
TEAM_A_BSS_POSITIONS = BSS_POS_LIST(
    pitchPanelCoordinate(Vector2 {
        .x = (PITCH_PANEL_WIDTH / 4.0) + GOAL_OFFSET, // wtf with needing to add GOAL_OFFSET?
        .y = PITCH_PANEL_HEIGHT / 2.0
    })
);

constexpr std::array<Vector2, BSS_SLOTS_HORIZONTAL * BSS_SLOTS_VERTICAL>
TEAM_B_BSS_POSITIONS = BSS_POS_LIST(
    pitchPanelCoordinate(Vector2 {
        .x = (3.0 * PITCH_PANEL_WIDTH) / 4.0,
        .y = PITCH_PANEL_HEIGHT / 2.0
    })
);

#endif
