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

// MARK: Home Region Coordinate Conversion

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

template<>
struct std::hash<IVec2> {
    std::size_t operator()(const IVec2& v) const noexcept {
        auto hash = std::hash<int>{}(v.x);
        // hash combine algorithm from Boost - no clue how this works...
        hash ^= std::hash<int>{}(v.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        return hash;
    }
};

// TODO: Should Neighborhood be used? If so, should it incorporate field
// regions?
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

struct Neighborhoodable {
    IVec2 lastSetICoordinates;
};

/* === AI Behavior Definitions ============================================= */

enum struct FieldPlayerState {
    /*
        WAIT at steering position, and correct if jostled by player collision

        WHEN has just kicked ball
        UNTIL
        - upfield of player controlling ball in order to advance ball, OR
        - ball is closest to player and no other receiving player
    */
    WAIT,
    /*
        RECEIVE_BALL, go to intercept the ball. Either by:
        - _Arrive_ if no urgency, OR
        - _Pursuit_ if urgent. (See book formula)
        
        WHEN player is instructed to RECEIVE_BALL
        UNTIL
        - close to the ball, OR
        - opposing team has control of the ball
        THEN CHASE_BALL
    */
    RECEIVE_BALL,
    /*
        _Seek_ to the ball's current position

        WHEN
        - waiting and closest w/ no receiver, OR
        - wants to kick, but exceeded frequency, OR
        - after dribble kick (case of above?)

        UNTIL within kicking range -> KICK_BALL
    */
    CHASE_BALL,
    /*
        RETURN_TO_HOME_REGION 
        WHEN signalled to return
        UNTIL ???
    */
    RETURN_TO_HOME_REGION,
    /*
        KICK_BALL procedure
        1. Are we timed out of kick? If so, -> DRIBBLE
        2. Are we behind the ball? If not, -> CHASE_BALL
        3. We are able to kick the ball.
        4a. If good shot possible, do so.
        4b. If no goal is possible, and threatened by another player, pass ball
        to upfield player which is safest and tell it to RECEIVE_BALL. 
            -> WAIT (?)
        4c. If not threatened, move to step 5.
        4d. Kick, -> WAIT.
        5. If no threat, no goal shot possible -> DRIBBLE
        
        WHEN ball is within kicking distance of the player
    */
    KICK_BALL,
    /*
        Kick the ball in alternating PI / 4 rotations softly

        WHEN controlling ball, not threatened, no pass or goal possible.
        THEN -> CHASE_BALL (after dribble kick)
    */
    DRIBBLE,
    /*
        WHEN nearest player to BSS for currently controlling player:
        - _Arrive_ at BSS
        - Ensure facing ball
        - If goal shot possible, request pass from currently controlling player.
        UNTIL ball received, -> RECEIVE_BALL
        OR lose control of ball, RETURN_TO_HOME_REGION
    */
    SUPPORT_ATTACKER
};

enum struct GoalkeeperState {
    /*
        Goalkeeper will _interpose_ between the ball and the goal line (rear),
        following a semicircle radius

        WHEN done with INTERCEPT_BALL
        OR done with PUT_BALL_BACK_IN_PLAY
        (this state is the default behavior of the goalkeeper)
    */
    TEND_GOAL,
    /*
        Return to home region
        UNTIL ???
    */
    RETURN_HOME,
    /*
        1. Set all team players to home region and wait for them all to arrive.
        2. Try to make pass as able.
        3. Return to TEND_GOAL
    */
    PUT_BALL_BACK_IN_PLAY,
    /*
        INTERCEPT_BALL if the opponent has control of the ball and are within
        threat range. Follow ball with _Pursuit_.

        UNTIL out of goal range, -> ReturnHome
            UNLESS closest player to ball, then continue _Pursuit_ out of 
            range.
        OR get ball, trap it, RETURN_HOME, PUT_BALL_BACK_IN_PLAY.

    */
    INTERCEPT_BALL
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
            BLACK
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
