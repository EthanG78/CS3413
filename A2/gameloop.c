#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


#include "gameloop.h"
#include "console.h"
#include "globals.h"

void animatePlayerTest();

void executeGameLoop()
{
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)) // start the game (maybe need to do this elsewhere...)
    {
        animatePlayerTest();
        finalKeypress(); /* wait for final key before killing curses and game */
    }
    consoleFinish();
}

// SEGAFAULTS
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