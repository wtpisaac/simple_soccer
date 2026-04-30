#ifndef SOCCER_MATH_HPP
#define SOCCER_MATH_HPP

#include <raylib.h>
#include <sys/types.h>
#include "constants.hpp"

/* === Pitch Panel ========================================================= */
// Coordinate conversion for pitch panel
template <typename T>
constexpr T
pitchPanelX(
    T x
) {
    return x + PITCH_PANEL_X;
}

template <typename T>
constexpr T
pitchPanelY(
    T y
) {
    return y + PITCH_PANEL_Y;
}

constexpr Vector2
pitchPanelCoordinate(
    Vector2 v
);

Vector2
pitchRegionPosition(
    uint homeRegion
);

/* === General Math ======================================================== */
float rotationFromVector(
    Vector2 vec
);

#endif