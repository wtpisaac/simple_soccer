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
