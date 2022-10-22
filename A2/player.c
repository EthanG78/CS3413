#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#define __USE_XOPEN2k
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "player.h"
#include "globals.h"
#include "console.h"

// There are two annimation frames for the player
// The player has width three and height threee
//   A   M
//  <x> <X>
//   V   V
char *PLAYER_BODY[PLAYER_BODY_ANIM_TILES][PLAYER_HEIGHT] =
    {
        {" A ",
         "<x>",
         " V "},
        {" M ",
         "<X>",
         " V "}};

int initPlayer()
{
    PLAYER_POS_X = 0;
    PLAYER_POS_Y = 0;

    // Move player to starting position
    if (!movePlayer(GAME_COLS / 2, 19))
    {
        // todo:
        // if movePlayer() returns 0, there was an issue
    }

    return 1;
}

int movePlayer(int deltaX, int deltaY)
{
    int errorCode = 0;

    errorCode = pthread_mutex_lock(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    int newPosX = PLAYER_POS_X + (int)floor((double)deltaX);
    if (newPosX <= GAME_COLS && newPosX >= 0)
    {
        PLAYER_POS_X = newPosX;
    }

    // I am subtracting deltaY because I want a -deltaY to
    // make the player go down in rows, but in reality it will
    // be going up in rows.
    int newPosY = PLAYER_POS_Y - (int)floor((double)deltaY);
    if (newPosY <= GAME_ROWS && newPosY > 16)
    {
        PLAYER_POS_Y = newPosY;
    }

    errorCode = pthread_mutex_unlock(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

void *playerController(void *x)
{
    // required for pselect call to stdin
    fd_set readfds;
    int ret;
    int errorCode;
    char inputChar;

    while (IS_RUNNING)
    {
        // Need to set stdin file descriptor
        // on each loop as pselect may modify it
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        struct timespec timeout = getTimeout(1); /* duration of one tick */
        ret = pselect(FD_SETSIZE, &readfds, NULL, NULL, &timeout, NULL);

        if (ret == -1)
        {
            // todo:
            // Since we redefine readfds and
            // timeout on each loop, I don't think
            // we need to exit here?
            perror("pselect()");
        }
        else if (IS_RUNNING && ret >= 1)
        {
            inputChar = getchar();

            switch (inputChar)
            {
            case MOVE_DOWN:
                movePlayer(0, -1);
                break;
            case MOVE_UP:
                movePlayer(0, 1);
                break;
            case MOVE_LEFT:
                movePlayer(-1, 0);
                break;
            case MOVE_RIGHT:
                movePlayer(1, 0);
                break;
            case QUIT:
                errorCode = pthread_mutex_lock(&M_IsRunningCV);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_lock()");
                }

                errorCode = pthread_cond_signal(&IsRunningCv);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_cond_signal()");
                }

                // DO I NEED TO UNLOCK THE MUTEX??

                break;
            default:
                // user entered input
                // we do not handle
                break;
            }
        }
    }

    pthread_exit(NULL);
}

void *animatePlayer(void *idleTicks)
{
    int errorCode = 0;
    int nTicksPerAnimFrame = *(int *)idleTicks;

    if (!initPlayer())
    {
        // todo:
        // there was a problem
        pthread_exit(NULL);
    }

    while (IS_RUNNING)
    {
        for (int j = 0; j < PLAYER_BODY_ANIM_TILES; j++)
        {
            char **frame = PLAYER_BODY[j];

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            errorCode = pthread_mutex_lock(&M_PlayerPos);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            // clear the last drawing
            consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(frame[0]));
            // draw the player
            consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, frame, PLAYER_HEIGHT);

            errorCode = pthread_mutex_unlock(&M_PlayerPos);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }

            // sleep nTicksPerAnimFrame * 20ms
            sleepTicks(nTicksPerAnimFrame);
        }
    }

    pthread_exit(NULL);
}