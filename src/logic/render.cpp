#include <raylib.h>
#include <raygui.h>

#include "render.hpp"
#include "../core/constants.hpp"
#include "../core/math.hpp"
#include "../logic/components.hpp"

/* === Rendering =========================================================== */

void renderUI(
    GameState &gameState
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

    // Draw team scores
    DrawText(
        TextFormat("Team A: %u", gameState.teamAGoals),
        PITCH_PANEL_PADDING_SIDES,
        PITCH_PANEL_HEIGHT + PITCH_PANEL_PADDING_TOP + 32,
        32,
        BLACK
    );
    DrawText(
        TextFormat("Team B: %u", gameState.teamBGoals),
        PITCH_PANEL_PADDING_SIDES,
        PITCH_PANEL_HEIGHT + PITCH_PANEL_PADDING_TOP + 64 + 16,
        32,
        BLACK
    );
}

void renderBall(
    GameState &gameState
) {
    auto& registry = gameState.registry;
    auto& pos = registry.get<Position>(gameState.ball);

    DrawCircle(
        pos.pos.x,
        pos.pos.y,
        22,
        DARKGRAY
    );
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
    GameState &gameState
) {
    auto& registry = gameState.registry;
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

    // Render entities
    renderBall(gameState);
    renderPlayers(registry);

    EndScissorMode();
}

void render(
    GameState &gameState
) {
    BeginDrawing();
    ClearBackground(RED);

    renderUI(gameState);
    renderGame(gameState);

    EndDrawing();
}