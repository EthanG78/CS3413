#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "console.h"
#include "globals.h"
#include "gameloop.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"

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

// Initialize required data, mutexes, etc for game loop.
//
// Return 1 indicating success, error otherwise. In main
// program, do not call executeGameloop() unless this function
// returns success.
int initializeGameLoop()
{
    // set game state to running
    IS_RUNNING = 1;

    // set game related info
    PLAYER_LIVES_REMAINING = PLAYER_MAX_LIVES;
    PLAYER_SCORE = 0;

    // seed random number generator
    srand((unsigned int)time(NULL));

    // init global mutexes
    pthread_mutexattr_t errAttr;

    int errorCode = 0;
    errorCode = pthread_mutexattr_init(&errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutexattr_init()");
        return 0;
    }

    // we want mutexes to check for errors
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

    errorCode = pthread_mutex_init(&M_EnemyList, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_BulletList, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_DestroyBullets, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_PlayerLives, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_PlayerScore, &errAttr);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_init()");
        return 0;
    }

    errorCode = pthread_mutex_init(&M_IsPlayerHit, &errAttr);
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

// Cleanup mutexes that were initialized in initializeGameLoop().
// This function must be called after executeGameLoop() is called.
//
// Return 1 indicating success, error otherwise.
int cleanupGameLoop()
{
    int errorCode = 0;

    // todo: look at doig this on the fly in maintain thread
    // Cleanup all bullets.
    if (!cleanupBullets())
        return 0;

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

    errorCode = pthread_mutex_destroy(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_DestroyBullets);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_PlayerLives);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_PlayerScore);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_destroy()");
        return 0;
    }

    errorCode = pthread_mutex_destroy(&M_IsPlayerHit);
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

// This is a utility function that signals the IsRunningCV
// condition variable to end the gameloop. This is called
// when a player wins, or loses.
//
// Returns 1 on success, error otherwise.
int signalGameOver()
{
    int errorCode = 0;
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

// Function that is responsible for refreshing the console
// each refreshRate ticks. This function will run in its own thread.
//
// Runs until IS_RUNNING is false and exits with
// pthread_exit(NULL).
void *refreshGameLoop(void *refreshRate)
{
    int errorCode = 0;
    int nTicksPerRefresh = *(int *)refreshRate;

    while (IS_RUNNING)
    {
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

// Function that is responsible for maintaining and checking
// certain aspects of the game while the game is running. Will check
// how many player lives are remaining, and will signal the game to quit
// if player lives reaches zero. Will check how many enemies are left, and
// will signal the game to quit if enemiesRemaining() returns 0. Will check
// if an enemy has made it to the bottom of the screen, and will signal the
// game to quit if enemyAtBottom() returns 1.
//
// Runs until IS_RUNNING is false and exits with
// pthread_exit(NULL).
void *maintainGameLoop(void *checkRate)
{
    int errorCode = 0;
    int nTicksPerCheck = *(int *)checkRate;

    char scoreStr[SCORE_MAX_LEN];
    char livesStr[LIVES_MAX_LEN];

    while (IS_RUNNING)
    {
        if (PLAYER_SCORE <= 999)
        {
            errorCode = pthread_mutex_lock(&M_PlayerScore);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            sprintf(scoreStr, "%d", PLAYER_SCORE);

            errorCode = pthread_mutex_unlock(&M_PlayerScore);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            putString(scoreStr, SCORE_ROW, SCORE_COL, SCORE_MAX_LEN);

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }
        }

        if (PLAYER_LIVES_REMAINING >= 0)
        {
            errorCode = pthread_mutex_lock(&M_PlayerLives);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            sprintf(livesStr, "%d", PLAYER_LIVES_REMAINING);

            errorCode = pthread_mutex_unlock(&M_PlayerLives);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            putString(livesStr, LIVES_ROW, LIVES_COL, LIVES_MAX_LEN);

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }
        }

        errorCode = pthread_mutex_lock(&M_PlayerLives);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        if (PLAYER_LIVES_REMAINING == 0 || enemyAtBottom() == 1)
        {
            // Player has died, let them know and quit!
            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                return 0;
            }

            putBanner("You lose!");

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                return 0;
            }

            if (!signalGameOver())
                printf("UNABLE TO SIGNAL GAME OVER");
        }

        errorCode = pthread_mutex_unlock(&M_PlayerLives);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        if (enemiesRemaining() == 0)
        {
            // Player has killed all enemies! They win!

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                return 0;
            }

            // todo:
            // Sometimes when the game launches, we segfault
            // and display the YOU WIN text. I think it has to do with
            // us freeing caterpillars when they are not yet spawned in....
            putBanner("You win!");

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                return 0;
            }

            if (!signalGameOver())
                printf("UNABLE TO SIGNAL GAME OVER");
        }

        sleepTicks(nTicksPerCheck);
    }

    pthread_exit(NULL);
}

// Function that is responsible for launching each thread that
// the game requires.
//
// Returns 1 indicating success, error otherwise.
int launchThreads()
{
    int errorCode = 0;
    int nThreads = 5;
    pthread_t threads[nThreads];

    // Store functions to be run in their
    // own threads here
    void *(*threadFunctions[])(void *) = {
        &refreshGameLoop,
        &animatePlayer,
        &playerController,
        &enemySpawner,
        &maintainGameLoop};

    int refreshRate = 1;
    int playerIdleTicks = 15;
    int ticksPerEnemy = 2500;

    // Store function params at same
    // index in this array as the function
    // in threadFunctions array
    void *threadParams[] = {
        (void *)&refreshRate,
        (void *)&playerIdleTicks,
        NULL,
        (void *)&ticksPerEnemy,
        (void *)&playerIdleTicks};

    // call pthread_create for each of the threads in threads[], using each function
    // from threadFunctions[] with the parameters in threadParams[]. It is crucial
    // that the indexes of the functions and parameters line up in their arrays.
    for (int i = 0; i < nThreads; i++)
    {
        errorCode = pthread_create(&threads[i], NULL, threadFunctions[i], threadParams[i]);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_create()");
            return 0;
        }
    }

    // todo:
    // should I be joining the threads here?
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
    int errorCode = 0;

    // Start the game console
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD))
    {
        // Launch the required threads for the game to run
        if (!launchThreads())
        {
            // If we fail to launch the game threads, then we alert
            // the user and immediately close the gameloop.
            IS_RUNNING = 0;

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
            }

            putBanner("Unable to launch game threads. Exiting.");

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
            }
        }
        else
        {
            // When threads are successfully launched, we wait on
            // the IsRunningCV condition variable to tell us that 
            // the game is done running.
            errorCode = pthread_mutex_lock(&M_IsRunningCV);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
            }

            while (IS_RUNNING)
            {
                errorCode = pthread_cond_wait(&IsRunningCv, &M_IsRunningCV);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_cond_wait()");
                }
            }

            errorCode = pthread_mutex_unlock(&M_IsRunningCV);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
            }
        }

        // When the gameloop is done, we wait for one finalkeypress
        // before killing curses and game
        disableConsole(1);
        finalKeypress();
    }

    errorCode = pthread_mutex_lock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
    }

    consoleFinish();

    errorCode = pthread_mutex_unlock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
    }
}