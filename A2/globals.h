#ifndef GLOBALS_H
#define GLOBALS_H

// Required to access pthread_mutexattr_settype
// and PTHREAD_MUTEX_ERRORCHECK... Check pthread.h
#define __USE_UNIX98

#include <pthread.h>

/*
* Global Mutexes
*/
pthread_mutex_t M_Console;
pthread_mutex_t M_PlayerPos;

/*
* Game State Definitions
*/
int IS_RUNNING;

/*
* Control Definitions
*/
#define MOVE_LEFT 'a'
#define MOVE_RIGHT 'd'
#define MOVE_UP 'w'
#define MOVE_DOWN 's'
#define SHOOT ' '
#define QUIT 'q'

#define GAME_ROWS 24
#define GAME_COLS 80

/*
* Caterpillar Globals
*/

#define ENEMY_HEIGHT 2
#define ENEMY_BODY_ANIM_TILES 8 

// MODIFY MODEL
/*char* ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] = 
{
  {"1",
   "1"},
  {"2",
   "2"},
  {"3",
   "3"},
  {"4",
   "4"},
  {"5",
   "5"},
  {"6",
   "6"},
  {"7",
   "7"},
  {"8",
   "8"}
};*/

/*
* Player Globals
*/

#define PLAYER_HEIGHT 3
#define PLAYER_BODY_ANIM_TILES 2 

int PLAYER_POS_X;
int PLAYER_POS_Y;

#endif // GLOBALS_H