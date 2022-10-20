#include <math.h>

#include "player.h"
#include "globals.h"
#include "console.h"

int movePlayer(int deltaX, int deltaY)
{
    pthread_mutex_lock(&M_PlayerPos);

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

    pthread_mutex_unlock(&M_PlayerPos);
}

void *playerController(void *x)
{
    // here is where we handle input and call moveplayer

    // Move player to starting position
    movePlayer(GAME_COLS / 2, 19);
}

void *animatePlayer(void *idleTicks)
{
    int nTicksPerAnimFrame = *(int *)idleTicks;

    while (IS_RUNNING)
    {
        for (int j = 0; j < PLAYER_BODY_ANIM_TILES; j++)
        {
            char **frame = PLAYER_BODY[j];

            pthread_mutex_lock(&M_Console);
            pthread_mutex_lock(&M_PlayerPos);

            // clear the last drawing
            consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(frame[0]));
            // draw the player
            consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, frame, PLAYER_HEIGHT);

            pthread_mutex_unlock(&M_PlayerPos);
            pthread_mutex_unlock(&M_Console);

            // sleep nTicksPerAnimFrame * 20ms
            sleepTicks(nTicksPerAnimFrame);
        }
    }
}