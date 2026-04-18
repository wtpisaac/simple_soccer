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

#include "core/logger.hpp"

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
) {
    return Vector2 {
        v.x + PITCH_PANEL_X,
        v.y + PITCH_PANEL_Y
    };
}

/* === Pitch Region ======================================================== */

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

/* === State ========================================================== */
struct GameState {
    entt::registry registry;
};

/* === Components ===================================================== */

struct PlayerData {
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

/* === Standalone Methods ================================================== */

// TODO: Implement some of the mathematical computations from the book. We
// will want to understand them, but we should just take the prior work.

void initialize_field_and_players(
    entt::registry& registry
) {
    // Spawn field players and goalkeepers here
    // Goalkeepers at goal lines
    // Field players at top middle, top leading cells
    // that is, cell 12, 14, 11, 9, AND cell 8, 5, 6, 3
    // goalkeepers cell 1 and 16. 

    // Team A
    for(uint i = 0; i < TOTAL_PITCH_REGIONS; i++) {
        auto entity = registry.create();
        registry.emplace<Position>(entity, Position { .pos = pitchRegionPosition(i) });
        registry.emplace<PlayerData>(entity, PlayerData { .homeRegion = i });
    }
    
    // Team B
}

/* === Leader-Follower Tracking ============================================ */

struct GameState {
    entt::registry registry;
};

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

/* === Rendering =========================================================== */

void renderUI(
    bool& shouldExit
) {
    // TODO: Should build controls to control game state.
    GuiWindowBox(
        Rectangle { 
            .x = 0, 
            .y = 0, 
            .width = WINDOW_WIDTH, 
            .height = WINDOW_HEIGHT 
        },
        ""
    );
    shouldExit = false;
}

void renderPlayers(
    entt::registry& registry
) {
    auto view = registry.view<const PlayerData, const Position>();

    view.each([](const PlayerData &, const Position &pos) {
        DrawRectangleRec(
            Rectangle {
                .x = pos.pos.x,
                .y = pos.pos.y,
                .width = 44.0,
                .height = 44.0
            },
            BROWN
        );
    });
}

void renderGame(
    entt::registry& registry
) {
    // Draw bordered area under pitch 
    DrawRectangleRec(
        Rectangle {
            .x = PITCH_BORDER_X, 
            .y = PITCH_BORDER_Y,
            .width = PITCH_BORDER_WIDTH,
            .height = PITCH_BORDER_HEIGHT
        },
        DARKGRAY
    );
    BeginScissorMode(
        PITCH_PANEL_X, 
        PITCH_PANEL_Y,
        PITCH_PANEL_WIDTH,
        PITCH_PANEL_HEIGHT
    );

    ClearBackground(GREEN);

    // Draw goals
    // Left goal
    DrawLineEx(
        Vector2 { (float)pitchPanelX(0), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) + (PITCH_REGION_HEIGHT / 2)) },
        Vector2 { (float)pitchPanelX(GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) + (PITCH_REGION_HEIGHT / 2)) },
        2.0f, WHITE
    );
    DrawLineEx(
        Vector2 { (float)pitchPanelX(0), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) - (PITCH_REGION_HEIGHT / 2)) },
        Vector2 { (float)pitchPanelX(GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) - (PITCH_REGION_HEIGHT / 2)) },
        2.0f, WHITE
    );
    DrawLineEx(
        Vector2 { (float)pitchPanelX(GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) - (PITCH_REGION_HEIGHT / 2)) },
        Vector2 { (float)pitchPanelX(GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) + (PITCH_REGION_HEIGHT / 2)) },
        2.0f, WHITE
    );

    // Right Goal
    DrawLineEx(
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH - GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) + (PITCH_REGION_HEIGHT / 2)) },
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) + (PITCH_REGION_HEIGHT / 2)) },
        2.0f, WHITE
    );
    DrawLineEx(
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH - GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) - (PITCH_REGION_HEIGHT / 2)) },
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) - (PITCH_REGION_HEIGHT / 2)) },
        2.0f, WHITE
    );
    DrawLineEx(
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH - GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) - (PITCH_REGION_HEIGHT / 2)) },
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH - GOAL_OFFSET), (float)pitchPanelY((PITCH_PANEL_HEIGHT / 2) + (PITCH_REGION_HEIGHT / 2)) },
        2.0f, WHITE
    );
    // Draw center circle
    DrawRing(
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH / 2), (float)pitchPanelY(PITCH_PANEL_HEIGHT / 2) },
        CENTER_CIRCLE_RADIUS - 1.0f,
        CENTER_CIRCLE_RADIUS + 1.0f,
        0.0f, 360.0f, 64, WHITE
    );

    // Draw dividing line
    DrawLineEx(
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH / 2), (float)pitchPanelY(0) },
        Vector2 { (float)pitchPanelX(PITCH_PANEL_WIDTH / 2), (float)pitchPanelY(PITCH_PANEL_HEIGHT) },
        2.0f, WHITE
    );

    // Render players
    renderPlayers(registry);

    EndScissorMode();
}

void render(
    bool& shouldExit,
    entt::registry& registry
) {
    BeginDrawing();
    ClearBackground(RED);

    renderUI(shouldExit);
    renderGame(registry);

    EndDrawing();
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

    initialize_field_and_players(gameState.registry);

    // Run loop
    while(!WindowShouldClose() && !shouldExit) {
        processInput(gameState);
        update(gameState);
        render(
            shouldExit,
            gameState.registry
        );
    }

    // Close up
    SOCCER_LOG_INFO(
        "soccer closing!"
    );
    CloseWindow();
    return 0;
}
