#include <cmath>

#include "logic.hpp"

#include "../core/constants.hpp"
#include "../core/math.hpp"
#include "../physics.hpp"

bool
isPassSafe(
    Vector2 passOrigin,
    Vector2 passDestination,
    Vector2 defenderPosition
) {
    // Convert coordinate space from local to the passer's coordinate space
    // 1. Translate the origin to passer
    auto translateMatrix = MatrixTranslate(-passOrigin.x, -passOrigin.y, 0.0f);
    // 2. Rotate the coordinate space to the passer's orientation
    // TODO: Does the below reflect the orientation of the _pass_?
    // TODO: What axis do we want to be "forward" for purposes of getting a tangent (single axis change for defender?)?
    auto rotateMatrix = MatrixRotateZ(
        90 + 
        (-1.0 * rotationFromVector(Vector2Subtract(
            passDestination, 
            passOrigin
        )))
    );
    // 3. Combine the matrices
    auto combinedMatrix = MatrixMultiply(rotateMatrix, translateMatrix);
    // 4. Convert defender position to the passer's coordinate space
    auto defenderLocalPosition = Vector2Transform(defenderPosition, combinedMatrix);
    // 5. Filter out opponent if not ahead of player
    if(defenderLocalPosition.x < 0.0f) {
        return true;
    }
    // 6. Get time to distance tangential to opponent
    auto timeToTangential = Physics::TimeToCoverDistanceWithResettingForce(
        Vector2Zero(), 
        Vector2 {
            .x = defenderLocalPosition.x,
            .y = 0.0f
        },
        BALL_MASS,
        BALL_FRICTION,
        BALL_RESET_FORCE
    );

    auto opponentReach = (
        PLAYER_MAX_DISTANCE_PER_SECOND * timeToTangential
    )
    + BALL_RADIUS
    + PLAYER_RADIUS;

    return opponentReach < std::fabs(defenderLocalPosition.y);
}
