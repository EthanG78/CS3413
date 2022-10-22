#ifndef PLAYER_H
#define PLAYER_H

// Require this definition to use
// the pselect function
#define __USE_XOPEN2k

// functions we need:
//  - Capture user input with pselect
//  - Move player based on user input
//  - Run player animation

int movePlayer(int deltaX, int deltaY);

void *playerController(void *x);

void *animatePlayer(void *idleTicks);

#endif // PLAYER_H
