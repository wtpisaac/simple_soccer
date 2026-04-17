/*
    GoalkeeperState.hpp - Goalkeeper components relating to player state.

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

/*
    Goalkeeper will _interpose_ between the ball and the goal line (rear),
    following a semicircle radius

    WHEN done with INTERCEPT_BALL
    OR done with PUT_BALL_BACK_IN_PLAY
    (this state is the default behavior of the goalkeeper)
*/
struct GoalkeeperTendGoal {};

/*
    Return to home region
    UNTIL ???
*/
struct GoalkeeperReturnToHomeRegion {};

/*
    1. Set all team players to home region and wait for them all to arrive.
    2. Try to make pass as able.
    3. Return to TEND_GOAL
*/
struct GoalkeeperPutBallBackInPlay {};

/*
    INTERCEPT_BALL if the opponent has control of the ball and are within
    threat range. Follow ball with _Pursuit_.

    UNTIL out of goal range, -> ReturnHome
        UNLESS closest player to ball, then continue _Pursuit_ out of 
        range.
    OR get ball, trap it, RETURN_HOME, PUT_BALL_BACK_IN_PLAY.

*/
struct GoalkeeperInterceptBall {};
