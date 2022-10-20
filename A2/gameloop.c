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
    IS_RUNNING = 0;

    // init global mutexes

    return 1;
}

void executeGameLoop()
{
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)) // start the game (maybe need to do this elsewhere...)
    {
        IS_RUNNING = 1;

        // TODO: Figure out cond variable and who joins threads

        // animatePlayerTest();
        putBanner("Game Over");
        finalKeypress(); /* wait for final key before killing curses and game */
    }
    consoleFinish();

    IS_RUNNING = 0;
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
            return NULL;
        }

        consoleRefresh();

        if (pthread_mutex_unlock(&M_Console) != 0)
        {
            perror("pthread_mutex_unlock()");
            return NULL;
        }

        sleepTicks(nTicksPerRefresh);
    }

    return NULL;
}

int cleanupGameLoop()
{
    // destroy mutexes and stuff
    return 1;
}