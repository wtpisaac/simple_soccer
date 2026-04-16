
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
