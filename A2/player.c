#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>

#include "player.h"
#include "globals.h"
#include "console.h"

// There are two annimation frames for the player
// The player has width three and height threee
//   A   M
//  <x> <X>
//   V   V
char* PLAYER_BODY[PLAYER_BODY_ANIM_TILES][PLAYER_HEIGHT] = 
{
  {" A ",
   "<x>",
   " V "},
  {" M ",
   "<X>",
   " V "}
};

int initPlayer()
{
    PLAYER_POS_X = 0;
    PLAYER_POS_Y = 0;

    return 1;
}

int movePlayer(int deltaX, int deltaY)
{
    if (pthread_mutex_lock(&M_PlayerPos) != 0)
    {
        perror("pthread_mutex_lock()");
        return 0;
    }

    int newPosX = PLAYER_POS_X + floor(deltaX);
    if (newPosX <= GAME_COLS && newPosX >= 0)
    {
        PLAYER_POS_X = newPosX;
    }

    int newPosY = PLAYER_POS_Y + floor(deltaY);
    if (newPosY <= GAME_ROWS && newPosY > 16)
    {
        PLAYER_POS_Y = newPosY;
    }

    if (pthread_mutex_unlock(&M_PlayerPos) != 0)
    {
        perror("pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

void *playerController(void *x)
{
    if (!initPlayer())
    {
        // todo:
        // there was a problem
        return NULL;
    }

    // here is where we handle input and call moveplayer

    // Move player to starting position
    if (!movePlayer(GAME_COLS / 2, 19))
    {
        // todo:
        // if movePlayer() returns 0, there was an issue
    }

    return NULL;
}

void *animatePlayer(void *idleTicks)
{
    int nTicksPerAnimFrame = *(int *)idleTicks;

    while (IS_RUNNING)
    {
        for (int j = 0; j < PLAYER_BODY_ANIM_TILES; j++)
        {
            char **frame = PLAYER_BODY[j];

            if (pthread_mutex_lock(&M_Console) != 0)
            {
                perror("pthread_mutex_lock()");
                return NULL;
            }

            if (pthread_mutex_lock(&M_PlayerPos) != 0)
            {
                perror("pthread_mutex_lock()");
                return NULL;
            }

            // clear the last drawing
            consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(frame[0]));
            // draw the player
            consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, frame, PLAYER_HEIGHT);

            if (pthread_mutex_unlock(&M_PlayerPos) != 0)
            {
                perror("pthread_mutex_unlock()");
                return NULL;
            }

            if (pthread_mutex_unlock(&M_Console) != 0)
            {
                perror("pthread_mutex_unlock()");
                return NULL;
            }

            // sleep nTicksPerAnimFrame * 20ms
            sleepTicks(nTicksPerAnimFrame);
        }
    }

    return NULL;
}