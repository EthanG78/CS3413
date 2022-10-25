#ifndef PLAYER_H
#define PLAYER_H

int movePlayer(int deltaX, int deltaY);

void *playerController(void *x);

void *animatePlayer(void *idleTicks);

#endif // PLAYER_H
