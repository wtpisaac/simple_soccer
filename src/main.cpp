/*
    implementing simple soccer using ecs and raylib copyright (c) 2026 isaac
    trimble-pederson methods and design taken from Game AI by Example by Mat
    Buckland

    ===

    Copyright (c) 2026 Isaac Trimble-Pederson

    This program is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the Free
    Software Foundation, either version 3 of the License, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
    more details.

    You should have received a copy of the GNU General Public License along with
    this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdlib>

#include <ankerl/unordered_dense.h>
#include <entt/entt.hpp>

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <raymath.h>

#include <quill/LogMacros.h>

#include "core/constants.hpp"
#include "core/math.hpp"
#include "core/logger.hpp"
#include "states/FieldPlayerState.hpp"
#include "states/GameState.hpp"
#include "logic/components.hpp"
#include "logic/render.hpp"

/* === Standalone Methods ================================================== */

// TODO: Implement some of the mathematical computations from the book. We
// will want to understand them, but we should just take the prior work.

void initialize_field_and_players(
    GameState &gameState
) {
    auto &registry = gameState.registry;
    // Spawn field players and goalkeepers here
    // Goalkeepers at goal lines
    // Field players at top middle, top leading cells
    // that is, cell 12, 14, 11, 9, AND cell 8, 5, 6, 3
    // goalkeepers cell 1 and 16.

    // Team A
    auto teamA = registry.create();

    // Field players
    const uint teamAFieldPlayerRegions[] {14, 11, 12, 9};
    for(auto region: teamAFieldPlayerRegions) {
        auto entity = registry.create();
        registry.emplace<Position>(entity, Position { .pos = pitchRegionPosition(region) });
        registry.emplace<PlayerData>(entity, PlayerData {
            .team = teamA,
            .homeRegion = region
        });
        registry.emplace<FieldPlayerStateWait>(
            entity,
            FieldPlayerStateWait {
                .at = pitchRegionPosition(region)
            }
        );
    }
    // Goalkeeper
    auto teamAGoalkeeper = registry.create();
    registry.emplace<Position>(teamAGoalkeeper, Position { .pos = pitchRegionPosition(16) });
    registry.emplace<PlayerData>(teamAGoalkeeper, PlayerData {
        .team = teamA,
        .homeRegion = 16
    });

    // Team B
    entt::entity teamB = registry.create();

    const uint teamBFieldPlayerRegions[] {8, 5, 6, 3};
    for(auto region: teamBFieldPlayerRegions) {
        auto entity = registry.create();
        registry.emplace<Position>(entity, Position { .pos = pitchRegionPosition(region) });
        registry.emplace<PlayerData>(entity, PlayerData {
            .team = teamB,
            .homeRegion = region
        });
        registry.emplace<FieldPlayerStateWait>(
            entity,
            FieldPlayerStateWait {
                .at = pitchRegionPosition(region)
            }
        );
    }
    // Goalkeeper
    auto teamBGoalkeeper = registry.create();
    registry.emplace<Position>(teamBGoalkeeper, Position { .pos = pitchRegionPosition(1) });
    registry.emplace<PlayerData>(teamBGoalkeeper, PlayerData {
        .team = teamB,
        .homeRegion = 1
    });

    // Ball
    auto ball = registry.create();
    registry.emplace<Position>(
        ball,
        Position {
            .pos = Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH / 2), (float)pitchPanelY(PITCH_PANEL_HEIGHT / 2) }
        }
    );
    gameState.ball = ball;

    // Store teams
    gameState.teams[0] = teamA;
    gameState.teams[1] = teamB;
}

/* === Input =============================================================== */

void processInput(
    GameState &gameState
) {
}

/* === Logic =============================================================== */

void acceleration_system(
    entt::registry &registry
) {
    auto view = registry.view<Velocity>();

    view.each([&registry](Velocity &vel) {
        auto magnitude = Vector2Length(vel.velocity);
        auto normalized = Vector2Normalize(vel.velocity);

        // TODO: New velocities where relevant, new sums

        auto newVelocity = Vector2Add(vel.velocity, Vector2Zero());
        vel.velocity = newVelocity;
    });
}

void apply_velocity_to_position_system(
    entt::registry& registry
) {
    auto view = registry.view<Position, const Velocity>();

    view.each([&registry](Position &pos, const Velocity &vel) {
        pos.pos = Vector2Add(
            pos.pos,
            vel.velocity
        );
    });
}

void update(
    GameState &gameState
) {
    acceleration_system(gameState.registry);
    apply_velocity_to_position_system(gameState.registry);
}

/* === Main Loop =========================================================== */
int main()
{
    bool shouldExit {false};
    GameState gameState = {
        .registry = entt::registry()
    };

    SOCCER_LOG_INFO(
        "soccer!"
    );

    // Start Raylib rendering
    InitWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        APPLICATION_TITLE
    );
    SetTargetFPS(60);

    initialize_field_and_players(gameState);

    // Run loop
    while(!WindowShouldClose() && !shouldExit) {
        processInput(gameState);
        update(gameState);
        render(
            gameState
        );
    }

    // Close up
    SOCCER_LOG_INFO(
        "soccer closing!"
    );
    CloseWindow();
    return 0;
}
