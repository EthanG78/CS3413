#include "player.h"
#include "globals.h"
#include "console.h"

int movePlayer(int deltaX, int deltaY)
{
    pthread_mutext_lock(&mPlayerPos);

    playerPosX += deltaX;
    playerPosY += deltaY;

    pthread_mutext_unlock(&mPlayerPos);
}

void *playerController(void *x)
{
    // here is where we handle input and call moveplayer
}

void *animatePlayer(void *tickRate)
{
    int sleepTime = 1 / (*(int *)tickRate);

    for (int j = 0; j < PLAYER_BODY_ANIM_TILES; j++)
    {
        char **frame = PLAYER_BODY[j];

        pthread_mutex_lock(&mConsole);
        pthread_mutext_lock(&mPlayerPos);

        consoleClearImage(playerPosY, playerPosX, PLAYER_HEIGHT, strlen(frame[0])); // clear the last drawing
        consoleDrawImage(playerPosY, playerPosX, frame, PLAYER_HEIGHT);             // draw the player
        consoleRefresh();                                                           // reset the state of the console drawing tool

        pthread_mutext_unlock(&mPlayerPos);
        pthread_mutex_unlock(&mConsole);

        // sleep based on the provided tickrate
        sleep(sleepTime);
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