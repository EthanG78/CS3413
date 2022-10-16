#include "gameloop.h"
#include "console.h"
#include "globals.h"

void executeGameLoop()
{
    if (consoleInit(GAME_ROWS, GAME_COLS, GAME_BOARD)) // start the game (maybe need to do this elsewhere...)
    {
        animatePlayerTest();
        finalKeypress(); /* wait for final key before killing curses and game */
    }
    consoleFinish();
}

void animatePlayerTest()
{
    int col = GAME_COLS / 2;
    int row = GAME_ROWS / 2;

    // loop over the whole enemy animation ten times
    // this "animation" is just the body changing appearance (changing numbers)
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; i < PLAYER_BODY_ANIM_TILES; j++)
        {
            char ***frame = PLAYER_BODY[j];

            for (int k = 0; k < PLAYER_WIDTH; k++)
            {
                consoleClearImage(row, col + k, PLAYER_HEIGHT, strlen(frame[k])); // clear the last drawing (why is this necessary?)
                consoleDrawImage(row, col + k, frame[k], PLAYER_HEIGHT);          // draw the player
                consoleRefresh();                                                 // reset the state of the console drawing tool
            }

            sleep(1); // give up our turn on the CPU
        }
    }
}