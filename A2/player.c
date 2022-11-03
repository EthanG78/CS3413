#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "player.h"
#include "bullet.h"
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

char **currentPlayerFrame = PLAYER_BODY[0];

int movePlayer(int deltaX, int deltaY)
{
    int errorCode = 0;

    int newPosX = PLAYER_POS_X + deltaX;
    if (newPosX <= GAME_COLS - PLAYER_HEIGHT && newPosX >= 0)
    {
        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(PLAYER_BODY[0][0]));

        errorCode = pthread_mutex_lock(&M_PlayerPos);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        PLAYER_POS_X = newPosX;

        errorCode = pthread_mutex_unlock(&M_PlayerPos);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }

        consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, currentPlayerFrame, PLAYER_HEIGHT);

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }
    }

    // I am subtracting deltaY because I want a -deltaY to
    // make the player go down in rows, but in reality it will
    // be going up in rows.
    int newPosY = PLAYER_POS_Y - deltaY;
    if (newPosY <= GAME_ROWS - PLAYER_HEIGHT && newPosY > 16)
    {
        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(PLAYER_BODY[0][0]));

        errorCode = pthread_mutex_lock(&M_PlayerPos);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        PLAYER_POS_Y = newPosY;

        errorCode = pthread_mutex_unlock(&M_PlayerPos);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }

        consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, currentPlayerFrame, PLAYER_HEIGHT);

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }
    }

    return 1;
}

int initPlayer()
{
    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    PLAYER_POS_X = 0;
    PLAYER_POS_Y = 0;

    errorCode = pthread_mutex_unlock(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    errorCode = pthread_mutex_lock(&M_IsPlayerHit);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    IS_PLAYER_HIT = 0;

    errorCode = pthread_mutex_unlock(&M_IsPlayerHit);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    // Move player to starting position, if this fails
    // then what do we have to life for but nothing?
    if (!movePlayer((int)(GAME_COLS / 2), -19))
        return 0;

    return 1;
}

int playerHit()
{
    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_PlayerLives);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    // Decrement player lives
    PLAYER_LIVES_REMAINING--;

    errorCode = pthread_mutex_unlock(&M_PlayerLives);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    // Remove all bullets from screen
    if (!cleanupBullets())
        return 0;

    errorCode = pthread_mutex_lock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    /*errorCode = pthread_mutex_lock(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    // Clean the screen besides the player
    //consoleClearImage(0, 0, GAME_ROWS, GAME_COLS);
    //consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, currentPlayerFrame, PLAYER_HEIGHT);

    errorCode = pthread_mutex_unlock(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }*/

    sleepTicks(250);
    consoleClearImage(PLAYER_POS_Y, PLAYER_POS_X, PLAYER_HEIGHT, strlen(PLAYER_BODY[0][0]));

    errorCode = pthread_mutex_unlock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    /*disableConsole(true);
    sleepTicks(250);
    disableConsole(false);*/

    // Reset player position
    if (!initPlayer())
        return 0;

    return 1;
}

int playerQuit()
{
    int errorCode = 0;

    errorCode = pthread_mutex_lock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    putBanner("You're lame....");

    errorCode = pthread_mutex_unlock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    errorCode = pthread_mutex_lock(&M_IsRunningCV);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    // change game state and signal main thread
    IS_RUNNING = 0;

    errorCode = pthread_cond_signal(&IsRunningCv);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_cond_signal()");
        return 0;
    }

    errorCode = pthread_mutex_unlock(&M_IsRunningCV);
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
    int errorCode = 0;
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

            // If the player move, shoot, or quit function
            // returns error, we continue to next loop and
            // try to execute the user input again.
            switch (inputChar)
            {
            case MOVE_DOWN:
                if (!movePlayer(0, -1))
                    continue;
                break;
            case MOVE_UP:
                if (!movePlayer(0, 1))
                    continue;
                break;
            case MOVE_LEFT:
                if (!movePlayer(-1, 0))
                    continue;
                break;
            case MOVE_RIGHT:
                if (!movePlayer(1, 0))
                    continue;
                break;
            case SHOOT:
                // Fire the bullet from the center of the player, 1
                // tile above them.
                if (!fireBullet(PLAYER_POS_X + 1, PLAYER_POS_Y - 1, 1))
                    continue;

                // todo:
                // Increment player score each time we fire, for now
                errorCode = pthread_mutex_lock(&M_PlayerScore);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_lock()");
                    pthread_exit(NULL);
                }
                PLAYER_SCORE++;
                errorCode = pthread_mutex_unlock(&M_PlayerScore);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_unlock()");
                    pthread_exit(NULL);
                }
                break;
            case QUIT:
                if (!playerQuit())
                    continue;
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
            // Check to see if the player has been hit
            errorCode = pthread_mutex_lock(&M_IsPlayerHit);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                return 0;
            }

            // When the player is hit, call the playerHit function
            // before returning to this function.
            if (IS_PLAYER_HIT)
            {
                errorCode = pthread_mutex_unlock(&M_IsPlayerHit);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_unlock()");
                    return 0;
                }
                playerHit();
            }
            else
            {
                errorCode = pthread_mutex_unlock(&M_IsPlayerHit);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_unlock()");
                    return 0;
                }
            }

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            currentPlayerFrame = PLAYER_BODY[j];

            errorCode = pthread_mutex_lock(&M_PlayerPos);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            // draw the player
            consoleDrawImage(PLAYER_POS_Y, PLAYER_POS_X, currentPlayerFrame, PLAYER_HEIGHT);

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