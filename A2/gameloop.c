#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "console.h"
#include "globals.h"
#include "gameloop.h"
#include "player.h"

/**** DIMENSIONS MUST MATCH the ROWS/COLS */
char *GAME_BOARD[] = {
    "                   Score:          Lives:",
    "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-centipiede!=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"",
    "",
    "",
    "",
    "",
    "",
    "",
    ""};

int initializeGameLoop()
{
    // set game state to running
    IS_RUNNING = 0;

    // init global mutexes
    pthread_mutexattr_t errAttr;

    if (pthread_mutexattr_init(&errAttr) != 0)
    {
        perror("pthread_mutexattr_init()");
        return 0;
    }

    if (pthread_mutexattr_settype(&errAttr, PTHREAD_MUTEX_ERRORCHECK) != 0)
    {
        perror("pthread_mutexattr_settype()");
        return 0;
    }

    if (pthread_mutex_init(&M_Console, &errAttr) != 0)
    {
        perror("pthread_mutex_init()");
        return 0;
    }

    if (pthread_mutex_init(&M_PlayerPos, &errAttr) != 0)
    {
        perror("pthread_mutex_init()");
        return 0;
    }

    if (pthread_mutexattr_destroy(&errAttr) != 0)
    {
        perror("pthread_mutexattr_destroy()");
        return 0;
    }

    return 1;
}

void executeGameLoop()
{
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)) // start the game (maybe need to do this elsewhere...)
    {
        IS_RUNNING = 1;

        // TODO: Figure out cond variable and who joins threads

        // Might have to put mutex around this
        putBanner("Game Over");
        finalKeypress(); /* wait for final key before killing curses and game */
    }

    if (pthread_mutex_lock(&M_Console) != 0)
    {
        perror("pthread_mutex_lock()");
        pthread_exit(NULL);
    }

    consoleFinish();

    if (pthread_mutex_unlock(&M_Console) != 0)
    {
        perror("pthread_mutex_unlock()");
        pthread_exit(NULL);
    }

    if (!cleanupGameLoop())
    {
        printf("UNABLE TO CLEANUP GAME LOOP");
    }
}

void *refreshGameLoop(void *refreshRate)
{
    int nTicksPerRefresh = *(int *)refreshRate;
    while (IS_RUNNING)
    {
        // todo:
        // is sleepTicks() part of my critical section?

        if (pthread_mutex_lock(&M_Console) != 0)
        {
            perror("pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        consoleRefresh();

        if (pthread_mutex_unlock(&M_Console) != 0)
        {
            perror("pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        sleepTicks(nTicksPerRefresh);
    }

    pthread_exit(NULL);
}

int cleanupGameLoop()
{
    // clear game state
    IS_RUNNING = 0;

    // destroy mutexes and stuff
    if (pthread_mutex_destroy(&M_Console) != 0)
    {
        perror("pthread_mutex_destroy()");
        return 0;
    }

    if (pthread_mutex_destroy(&M_PlayerPos) != 0)
    {
        perror("pthread_mutex_destroy()");
        return 0;
    }

    return 1;
}