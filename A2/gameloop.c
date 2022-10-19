#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "gameloop.h"
#include "console.h"
#include "globals.h"

int initializeGameLoop()
{
    // init global mutexes 
    return 1;
}

void executeGameLoop()
{
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)) // start the game (maybe need to do this elsewhere...)
    {
        IS_RUNNING = 1;


        // TODO: FIgure out cond variable and who joins threads

        animatePlayerTest();
        putBanner("Game Over");
        finalKeypress(); /* wait for final key before killing curses and game */
    }
    consoleFinish();

    IS_RUNNING = 0;
}

int cleanupGameLoop()
{
    // destroy mutexes and stuff
    return 1;
}