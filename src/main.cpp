#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include <cstdlib>

#include <ankerl/unordered_dense.h>
#include <entt/entt.hpp>

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <raymath.h>

#include <quill/LogMacros.h>

#include "core/logger.hpp"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define ARENA_PANEL_PADDING_TOP 0
#define ARENA_PANEL_PADDING_SIDES 0
#define ARENA_PANEL_HEIGHT WINDOW_HEIGHT
#define ARENA_PANEL_WIDTH WINDOW_WIDTH
#define ARENA_PANEL_X 0 + ARENA_PANEL_PADDING_SIDES
#define ARENA_PANEL_Y ARENA_PANEL_PADDING_TOP

#define ENTITY_SIZE 20.0
#define ENTITY_INTERIOR_SCALE 0.8
#define ENTITY_ACCELERATION 0.25
#define ENTITY_DESIRED_MAX_SPEED 3.0

#define PRIORITY_WALL 0.8
#define PRIORITY_SPEED 0.2

#define NEIGHBORHOOD_CELL_SIZE 60.0

#define APPLICATION_TITLE "Simple Soccer"

struct IVec2 {
    int x;
    int y;
    bool operator==(const IVec2&) const = default;
};

template<>
struct std::hash<IVec2> {
    std::size_t operator()(const IVec2& v) const noexcept {
        auto hash = std::hash<int>{}(v.x);
        // hash combine algorithm from Boost - no clue how this works...
        hash ^= std::hash<int>{}(v.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

constexpr Vector2 ARENA_OFFSET = Vector2 {
    .x = ARENA_PANEL_WIDTH / 2.0,
    .y = ARENA_PANEL_HEIGHT / 2.0
};

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
    // shouldExit = GuiWindowBox(
    //     Rectangle { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT },
    //     APPLICATION_TITLE
    // );
    shouldExit = false;
}

void renderArena(
    entt::registry& registry
) {
    BeginScissorMode(
        ARENA_PANEL_X, 
        ARENA_PANEL_Y,
        ARENA_PANEL_WIDTH,
        ARENA_PANEL_HEIGHT
    );

    ClearBackground(GREEN);

    EndScissorMode();
}

void render(
    bool& shouldExit,
    entt::registry& registry
) {
    BeginDrawing();
    ClearBackground(RED);

    renderUI(shouldExit);
    renderArena(registry);

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
