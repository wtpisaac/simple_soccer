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
constexpr int WINDOW_HEIGHT = 720;

// Pitch 
constexpr int PITCH_PANEL_PADDING_TOP = 16;
constexpr int PITCH_PANEL_PADDING_SIDES = 16;
constexpr int PITCH_PANEL_HEIGHT = WINDOW_HEIGHT - (2 * PITCH_PANEL_PADDING_TOP);
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
constexpr int PITCH_REGION_WIDTH = PITCH_PANEL_WIDTH / PITCH_REGIONS_HORIZONTAL;
constexpr int PITCH_REGION_HEIGHT = PITCH_PANEL_HEIGHT / PITCH_REGIONS_VERTICAL;

// Pitch Lines
constexpr float GOAL_OFFSET = static_cast<float>(PITCH_REGION_WIDTH) / 3.0f;
constexpr float CENTER_CIRCLE_RADIUS = static_cast<float>(PITCH_REGION_WIDTH);

// Neighborhood Processing
constexpr float NEIGHBORHOOD_CELL_SIZE = 60.0;

// Application Title
constexpr const char* APPLICATION_TITLE = "Simple Soccer";

// Fundamental Structures
struct IVec2 {
    int x;
    int y;
    bool operator==(const IVec2&) const = default;
};

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

constexpr IVec2
pitchPanelCoordinate(
    IVec2 v
) {
    return IVec2 {
        .x = v.x + PITCH_PANEL_X,
        .y = v.y + PITCH_PANEL_Y
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

template<>
struct std::hash<IVec2> {
    std::size_t operator()(const IVec2& v) const noexcept {
        auto hash = std::hash<int>{}(v.x);
        // hash combine algorithm from Boost - no clue how this works...
        hash ^= std::hash<int>{}(v.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

class Neighborhood {
public:
    IVec2
    addEntityToNeighborhood(
        entt::entity entity,
        IVec2 iCoordinates
    ) {
        auto& list = this->map[iCoordinates];
        list.push_back(entity);
        return iCoordinates;
    }

    void
    removeEntityFromNeighborhood(
        entt::entity entity,
        IVec2 iCoordinates
    ) {
        auto& list = this->getNeighborhoodListRef(iCoordinates);
        std::erase(list, entity);
    }

    std::vector<entt::entity>&
    getNeighborhoodListRef(
        IVec2 coordinates
    ) {
        auto& list = this->map[coordinates];
        return list;
    }

    std::vector<entt::entity>&
    getNeighborhoodListRef(
        Vector2 coordinates
    ) {
        auto iCoordinates = vecToNeighborhoodIVec(coordinates);
        auto& list = this->map[iCoordinates];
        return list;
    }

    static
    IVec2 vecToNeighborhoodIVec(
        Vector2 fVec
    ) {
        auto x = static_cast<int>(fVec.x / NEIGHBORHOOD_CELL_SIZE);
        auto y = static_cast<int>(fVec.y / NEIGHBORHOOD_CELL_SIZE);

        return IVec2 {
            .x = x,
            .y = y
        };
    };
private:
    /*
        Making a sidenote - we would need to think about having this map more 
        carefully if we were parallelizing this, either by making sure the
        neighborhood was not updated during other operations, since the
        unordered dense does not preserve stability of pointers for mutating
        operations.

        In our case, we will update neighborhood positions in a distinct
        phase from making use of the array pointers, and those pointers
        will not live long, and we are not doing any parallel logic for
        this demo, so none of this matters too much.
    */

    ankerl::unordered_dense::map<IVec2, std::vector<entt::entity>> map;
};

/* === ECS Definitions ===================================================== */

struct Position {
    Vector2 pos;
};

struct Velocity {
    Vector2 velocity;
};

struct Neighborhoodable {
    IVec2 lastSetICoordinates;
};

/* AI Behavior Definitions

Space to take some notes from the book:

- Entities should have steering behaviors
- These should probably be weighted but choosing the weights is difficult
- You can do a prioritization algorithm to allocate forces instead. might be worth trying
that up front since balancing the weights dynamically seems like a harder variant of this
- probably worth creating a chaining behavior to push away from entities but target a tail?

start with random flights
*/

/* === Standalone Methods ================================================== */

/* === Leader-Follower Tracking ============================================ */

struct GameState {
    entt::registry registry;
    Neighborhood neighborhood;
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

// TODO: Deletion support for neighborhood system
// TODO: General deletion approach rethink

void update_neighborhoods_system(
    GameState &gameState
) {
    auto view = gameState.registry.view<const Position, Neighborhoodable>();

    view.each([&gameState](entt::entity entity, const Position &pos, Neighborhoodable &neighborhood) {
        IVec2 newICoordinates = Neighborhood::vecToNeighborhoodIVec(pos.pos);
        if(newICoordinates != neighborhood.lastSetICoordinates) {
            gameState.neighborhood.removeEntityFromNeighborhood(
                entity,
                neighborhood.lastSetICoordinates
            );
            gameState.neighborhood.addEntityToNeighborhood(
                entity,
                newICoordinates
            );
            neighborhood.lastSetICoordinates = newICoordinates;
        }
    });
}

void update(
    GameState &gameState
) {
    acceleration_system(gameState.registry);
    apply_velocity_to_position_system(gameState.registry);

    update_neighborhoods_system(gameState);
}

/* === Rendering =========================================================== */

void renderUI(
    bool& shouldExit
) {
    shouldExit = false;
}

void renderPitch(
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

    EndScissorMode();
}

void render(
    bool& shouldExit,
    entt::registry& registry
) {
    BeginDrawing();
    ClearBackground(RED);

    renderUI(shouldExit);
    renderPitch(registry);

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
