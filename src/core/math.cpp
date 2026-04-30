#include "math.hpp"
#include <cassert>
#include <raymath.h>

/* === Pitch Panel ========================================================= */

constexpr Vector2
pitchPanelCoordinate(
    Vector2 v
) {
    return Vector2 {
        v.x + PITCH_PANEL_X,
        v.y + PITCH_PANEL_Y
    };
}

Vector2
pitchRegionPosition(
    uint homeRegion
) {
    assert(homeRegion >= 0 && homeRegion < TOTAL_PITCH_REGIONS && "Home region out of bounds");

    auto distance = TOTAL_PITCH_REGIONS - (homeRegion + 1);
    auto pitchRegionX = distance / PITCH_REGIONS_VERTICAL;
    auto pitchRegionY = distance % PITCH_REGIONS_VERTICAL;

    auto x = (pitchRegionX * PITCH_REGION_WIDTH) + (PITCH_REGION_WIDTH / 2);
    auto y = (pitchRegionY * PITCH_REGION_HEIGHT) + (PITCH_REGION_HEIGHT / 2);

    return Vector2 {
        .x = static_cast<float>(x),
        .y = static_cast<float>(y)
    };
}

float rotationFromVector(
    Vector2 vec
) {
    auto nvec = Vector2Normalize(vec);
    auto rads = Vector2Angle(
        Vector2{.y = -1.0},
        nvec
    );
    auto degs = (rads * RAD2DEG);
    return degs;
}
