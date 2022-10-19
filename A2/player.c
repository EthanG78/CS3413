#include <math.h>

#include "player.h"
#include "globals.h"
#include "console.h"

int movePlayer(int deltaX, int deltaY)
{
    pthread_mutex_lock(&M_PlayerPos);

    PLAYER_POS_X += floor(deltaX);
    PLAYER_POS_Y += floor(deltaY);

    pthread_mutex_unlock(&M_PlayerPos);
}

void *playerController(void *x)
{
    // here is where we handle input and call moveplayer

    // Move player to starting position
    movePlayer(GAME_COLS / 2, 19);
}

void *animatePlayer(void *tickRate)
{
    int sleepTime = 1 / (*(int *)tickRate);

    while (IS_RUNNING)
    {
        for (int j = 0; j < PLAYER_BODY_ANIM_TILES; j++)
        {
            char **frame = PLAYER_BODY[j];

            pthread_mutex_lock(&M_Console);
            pthread_mutex_lock(&M_PlayerPos);

            consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(frame[0])); // clear the last drawing
            consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, frame, PLAYER_HEIGHT);             // draw the player

            // All refreshing must be done in own thread
            // consoleRefresh();

            pthread_mutex_unlock(&M_PlayerPos);
            pthread_mutex_unlock(&M_Console);

            // sleep based on the provided tickrate
            sleep(sleepTime);
        }
    }
}

void animatePlayerTest()
{
    int col = GAME_COLS / 2;
    int row = GAME_ROWS / 2;

    // loop over the whole enemy animation ten times
    // this "animation" is just the body changing appearance (changing numbers)
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < PLAYER_BODY_ANIM_TILES; j++)
        {
            char **frame = PLAYER_BODY[j];

            consoleClearImage(row, col, PLAYER_HEIGHT, strlen(frame[0])); // clear the last drawing (why is this necessary?)
            consoleDrawImage(row, col, frame, PLAYER_HEIGHT);             // draw the player
            consoleRefresh();                                             // reset the state of the console drawing tool

            sleep(1); // give up our turn on the CPU
        }
    }
}