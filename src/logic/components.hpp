#ifndef SOCCER_LOGIC_COMPONENTS_HPP
#define SOCCER_LOGIC_COMPONENTS_HPP

#include <raylib.h>
#include <sys/types.h>
#include <entt/entt.hpp>

// Tags for team membership
struct OfTeamA {};
struct OfTeamB {};

struct BssMarker {
    float score;
};

struct PlayerData {
    entt::entity team;
    uint homeRegion;
};

struct Position {
    Vector2 pos;
};

struct Rotation {
    float heading;
};

struct Velocity {
    Vector2 velocity;
};

/*
 * Note on Ball component:
 * this doesn't seem necessary? the teams transition states when they gain control,
 * is that necessary to reflect in a distinct component or is this naturally arising
 * behavior?
 * For now, I am not defining an explicit component.
 */

#endif