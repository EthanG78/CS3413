#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

/*
 * Error printing function adapted
 * from https://man7.org/linux/man-pages/man3/pthread_create.3.html
 */
#define print_error(e, msg) \
  errno = e;                \
  perror(msg);

/*
 * Global Mutexes
 */
pthread_mutex_t M_Console;
pthread_mutex_t M_PlayerPos;
pthread_mutex_t M_IsRunningCV;
pthread_mutex_t M_EnemyList;
pthread_mutex_t M_BulletList;

pthread_cond_t IsRunningCv;

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


/*
 * Game Globals
 */
#define GAME_ROWS 24
#define GAME_COLS 80

/*
 * Bullet Globals
 */
#define BULLET_HEIGHT 1
#define BULLET_ANIM_TILES 1

/*
 * Caterpillar Globals
 */
#define ENEMY_HEIGHT 2
#define ENEMY_ANIM_TILES 2
#define ENEMY_DEFAULT_LENGTH 20
#define ENEMY_MIN_LENGTH 5

/*
 * Player Globals
 */
#define PLAYER_HEIGHT 3
#define PLAYER_BODY_ANIM_TILES 2

// todo:
// the whole prev_pos thing
// kinda sucks, look at something
// better when you can.
int PLAYER_POS_X;
int PLAYER_PREV_POS_X;
int PLAYER_POS_Y;
int PLAYER_PREV_POS_Y;

#endif // GLOBALS_H