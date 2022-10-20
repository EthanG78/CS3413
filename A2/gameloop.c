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

    int errorCode = 0;

    errorCode = pthread_mutexattr_init(&errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutexattr_init()");
        return 0;
    }

    errorCode = pthread_mutexattr_settype(&errAttr, PTHREAD_MUTEX_ERRORCHECK);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutexattr_settype()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_Console, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_PlayerPos, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_IsRunningCV, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_cond_init(&IsRunningCv, NULL);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_cond_init()");
        return 0;
    }

    errorCode = pthread_mutexattr_destroy(&errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutexattr_destroy()");
        return 0;
    }

    return 1;
}

int launchThreads()
{
    int nThreads = 3;
    pthread_t threads[nThreads];

    int refreshRate = 1;

    int errorCode = 0;

    errorCode = pthread_create(&threads[0], NULL, refreshGameLoop, (void *)refreshRate);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_create()");
        return 0;
    }

    for (int i = 0; i < nThreads; i++)
    {
        errorCode = pthread_join(threads[i], NULL);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_join()");
            return 0;
        }
    }

    return 1;
}

void executeGameLoop()
{
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)) // start the game (maybe need to do this elsewhere...)
    {
        IS_RUNNING = 1;

        if (!launchThreads())
        {
            // Might have to put mutex around this
            putBanner("Unable to launch game threads. Exiting.");
        }
        else
        {
            // wait on condition variable to tell us we are done
            if (pthread_mutex_lock(&M_IsRunningCV) != 0)
            {
                perror("pthread_mutex_lock()");
            }

            if (pthread_cond_wait(&IsRunningCv, &M_IsRunningCV) != 0)
            {
                perror("pthread_cond_wait()");
            }

            if (pthread_mutex_unlock(&M_IsRunningCV) != 0)
            {
                perror("pthread_mutex_unlock()");
            }

            // Might have to put mutex around this
            putBanner("Game Over");
        }

        // wait for final key before killing curses and game
        finalKeypress();

        // clear game state
        IS_RUNNING = 0;
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
    int errorCode = 0;
    int nTicksPerRefresh = *(int *)refreshRate;
    while (IS_RUNNING)
    {
        // todo:
        // is sleepTicks() part of my critical section?

        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        consoleRefresh();

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        sleepTicks(nTicksPerRefresh);
    }

    pthread_exit(NULL);
}

int cleanupGameLoop()
{
    int errorCode = 0;

    // destroy mutexes and stuff
    errorCode = pthread_mutex_destroy(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_PlayerPos);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_IsRunningCV);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_cond_destroy(&IsRunningCv);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_cond_destroy()");
        return 0;
    }

    return 1;
}