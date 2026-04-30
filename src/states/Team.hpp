/*
    Team.hpp - team related components

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
    Fairly speculative. Not sure what belongs here. We need to break this up 
    into two teams. But probably not as objects in the book.

    It seems like ultimately there are relationships between components and 
    we may associate some relationship with the entities to the team, but
    the primary goal of the entities may be to be emplaced with relevant
    targets that simply make sense for the team. In that sense, maybe the
    team logic works independently and then modifies relevant players?
*/

#ifndef TEAM_STATE_H
#define TEAM_STATE_H

struct Team {};

struct TeamDefendingState {};
struct TeamAttackingState {};

#endif
