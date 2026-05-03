#ifndef SOCCER_LOGIC_HPP
#define SOCCER_LOGIC_HPP

#include <raylib.h>
#include <raymath.h>
bool
isPassSafe(
    Vector2 passOrigin,
    Vector2 passDestination,
    Vector2 defenderPosition
);

#endif