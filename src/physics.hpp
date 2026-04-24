#ifndef SOCCER_PHYSICS_HPP
#define SOCCER_PHYSICS_HPP

#include <raylib.h>

namespace Physics {
    Vector2
    FuturePosition(
        Vector2 currentPosition,
        Vector2 currentVelocity,
        double mass,
        double friction,
        double time
    );

    /*
        TODO: this is ball specific as force is treated as reset?
        well, for now, "with resetting force" is a generic of this.
    */
    double
    TimeToCoverDistanceWithResettingForce(
        Vector2 x0,
        Vector2 x1,
        double mass,
        double resettingForce
    );
}

#endif
