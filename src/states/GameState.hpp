#ifndef SOCCER_GAME_STATE_HPP
#define SOCCER_GAME_STATE_HPP

#include <entt/entt.hpp>

struct GameState {
    entt::registry registry;
    entt::entity ball {entt::null};
    entt::entity teams[2] {entt::null, entt::null};
    uint8_t teamAGoals {0};
    uint8_t teamBGoals {0};
};

#endif