#include "physics.hpp"
#include "raymath.h"

/*
    Compute future position.

    **WARNING: No guard against backwards friction prediction.** 
*/
Vector2
Physics::FuturePosition(
    Vector2 currentPosition,
    Vector2 currentVelocity,
    double mass,
    double friction,
    double time
) {
    // Friction acceleration is constant to parameter and opposite of velocity
    auto vFriction = Vector2Scale(
        Vector2Normalize(
            currentVelocity
        ),
        // scale by -1.0 to negate vector
        -1.0 * friction
    );
    // dX = (v_0 * dt) + (1/2)(a)(dt^2)
    auto dPosition = Vector2Add(
        Vector2Scale(
            currentVelocity,
            time
        ),
        Vector2Scale(
            vFriction,
            0.5 * (time * time)
        )
    );

    return dPosition;
}

double
TimeToCoverDistanceWithResettingForce(
    Vector2 x0,
    Vector2 x1,
    double mass,
    double friction,
    double resettingForce
) {
    // Negate friction for use in the equations
    friction = -friction;

    double speed = resettingForce / mass;
    double dist = Vector2Distance(x0, x1);

    // v^2 = v_0^2 + 2ax
    // v = sqrt(v_0^2 + 2ax)
    // sqrt of negative is nonreal solution
    double term = speed*speed + (2.0 * dist * friction);
    if(term <= 0.0) {
        // no (real) solution
        return -1.0;
    }

    // solve for t
    // t = (v-v_0) / a
    double v = sqrt(term);
    return (v-speed) / friction;
}