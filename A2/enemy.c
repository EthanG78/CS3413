/*
  enemy.c
  CS3413 Operating Systems 1
  Assignment 2
  FALL 2022

  Author: Ethan Garnier
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

#include "enemy.h"
#include "bullet.h"
#include "globals.h"
#include "console.h"

// Caterpillar will look as follows:
//
//  o|~~~~~~~~~~~~
//    /'\'/'\'/'\'
//

// There are two animation frames for the head of the caterpillar.
// Since the caterpillar can face either left or right directions,
// we thus need 4 animation frames in total.
//  o|   -|
//
//
//  |o   |-
//
//
char *ENEMY_HEAD_LEFT[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
    {
        {"o|",
         "  "},
        {"-|",
         "  "}};

char *ENEMY_HEAD_RIGHT[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
    {
        {"|o",
         "  "},
        {"|-",
         "  "}};

// There are two animation frames for each body segment of the
// caterpillar. Each body segment has height two and width two.
char *ENEMY_BODY[ENEMY_ANIM_TILES][ENEMY_HEIGHT] =
    {
        {"~~",
         "/,"},
        {"~~",
         ",\\"}};

// Define a caterpillar struct that tracks the current row, col,
// length, direction of travel, and the number of ticks its sleeps
// per animation frame.
// This is what we will store in linked list of all caterpillars
// on screen.
typedef struct Caterpillar
{
    int col;
    int row;
    int length;
    int movingLeft;
    int sleeptTicks;
} Caterpillar;

// Define the node struct for our linked list of enemies.
// This node holds a reference to a caterpillar,
// a reference to a pthread_t where the enemy 'lives', and a
// reference to the next node in the list.
typedef struct EnemyNode
{
    Caterpillar *enemy;
    pthread_t *enemyThread;
    struct EnemyNode *next;
} EnemyNode;

// Define the head of our enemy linked list.
EnemyNode *enemyHead = NULL;

// This value keeps track if a caterpillar
// has made it to the player...
int enemyAtBottomFlag = 0;

// Function that initializes a new caterpillar at col x, row y, with a specific length,
// direction of travel, and number of idle ticks per animation.
//
// Returns 1 indicating success, error otherwise.
int spawnEnemy(int x, int y, int length, int isGoingLeft, int sleepTicks)
{
    // Need to malloc each
    // caterpillar so that it exists outside of this scope
    // and can be accessed as the linked list is accessed.
    Caterpillar *newEnemy = (Caterpillar *)malloc(sizeof(Caterpillar));
    newEnemy->col = x;
    newEnemy->row = y;
    newEnemy->length = length;
    newEnemy->movingLeft = isGoingLeft;
    newEnemy->sleeptTicks = sleepTicks;

    // Initialize the pthread that this new enemy will
    // execute on. We must allocate this on the stack.
    pthread_t *newEnemyThread = (pthread_t *)malloc(sizeof(pthread_t));

    // We must store this caterpillar in the list of enemies.
    // It stores this new enemy at the front of the linked list
    EnemyNode *newNode = (EnemyNode *)malloc(sizeof(EnemyNode));
    if (newNode == (EnemyNode *)NULL)
    {
        perror("malloc()");
        return 0;
    }

    newNode->enemy = newEnemy;
    newNode->enemyThread = newEnemyThread;
    newNode->next = enemyHead;

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    enemyHead = newNode;

    errorCode = pthread_mutex_unlock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

// Function that returns the number of enemy caterpillars
// currently alive in the game screen.
//
// Returns 1 indicating success, error otherwise.
int enemiesRemaining()
{
    // traverse the linked list, returning
    // the number of enemies currently on screen
    int length = 0;
    EnemyNode *current;

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    for (current = enemyHead; current != NULL; current = current->next)
    {
        // Count the number of enemies who have length greater than
        // the minimum, indicating they are still alive.
        if (current->enemy->length >= ENEMY_MIN_LENGTH)
            length++;
    }

    errorCode = pthread_mutex_unlock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return length;
}

// Function that returns the state of the
// enemyAtBottom flag. This is useful in
// the maintenance thread.
int enemyAtBottom()
{
    return enemyAtBottomFlag;
}

// Function that is responsible for animating the enemy
// caterpillar(s). This is the function that each caterpillar
// thread runs.
//
// Runs until IS_RUNNING is false and exits with
// pthread_exit(NULL).
void *animateEnemy(void *enemy)
{
    int errorCode = 0;
    Caterpillar *caterpillar = (Caterpillar *)enemy;

    // Nuumber of animation cycles we wait before the caterpillar
    // fires a bullet down from its head towards the player
    // Bounding this to [5 25]
    int nCyclesPerBullet = (rand() % 20) + 5;
    int bulletCounter = nCyclesPerBullet;

    // To determine where to draw each segment of the caterpillar,
    // I have come up with a way where we don't explicitly need to
    // increment/decrement rows/cols and care about the direction.
    // I number the caterpillar area as a grid as follows:
    //
    //  4  3  2  1
    //  5  6  7  8
    //  12 11 10 9          EXAMPLE 4x4 caterpillar grid
    //  13 14 15 16
    //
    // and store the caterpillar's head position (starts at 1 for new enemy).
    // Since we know the width of the caterpillar's head,  the width
    // of each body segment, and the length of the caterpillar, we
    // can determine the row/col to draw each segment based on the
    // numbered grid system and the location of the caterpillars head
    // in that grid system.

    // Required to account for each second row being skipped
    int rowOffset = (caterpillar->row / 2) - 1;

    // Position in grid of caterpillar head
    // This will be different based on if the caterpillar
    // was just spawned, or if it is from a split caterpillar.
    int caterpillarPos = (caterpillar->movingLeft)
                             ? (GAME_COLS - caterpillar->col) + ((caterpillar->row - 2 - rowOffset) * GAME_COLS)
                             : caterpillar->col + ((caterpillar->row - 2 - rowOffset) * GAME_COLS);

    // Position in grid of caterpillar body segment
    int segmentPos = 0;
    // Calculated column position of body segment
    int segmentCol = 0;
    // Calculated row position of body segment
    int segmentRow = 0;

    // The row and column of each segment can be transformed
    // from the caterpillarPos/segmentPos using the following math:
    //
    //  row = ceiling(pos / GAME_COLS) + 1 + offset
    //  - we add one since the caterpillar starts at index
    //    2 of the game board, and we add an offset to account for
    //    the fact that caterpillar skip each row (since they have height 2)
    //
    //  column = pos - ((row - 2 - offset) * GAME_COLS)
    //  - based on which direction the caterpillar is moving, we
    //    either use this as the column to draw the segment at, or we
    //    subtract this value from GAME_COLS and draw it at that column

    char **headFrame;
    char **bodyFrame;

    while (IS_RUNNING && caterpillar->row < 16 && caterpillar->length >= ENEMY_MIN_LENGTH)
    {
        headFrame = (caterpillar->movingLeft == 1)
                        ? ENEMY_HEAD_LEFT[(caterpillar->col & 1)]
                        : ENEMY_HEAD_RIGHT[(caterpillar->col & 1)];

        // Get the current row that the caterpillar is on
        caterpillar->row = (int)ceil((double)caterpillarPos / GAME_COLS) + 1 + rowOffset;

        // Based on which direction the caterpillar is facing, we either
        // subtract what column we are at from the max number of columns,
        // or just increment the columns
        caterpillar->col = (caterpillar->movingLeft == 1)
                               ? GAME_COLS - (caterpillarPos - ((caterpillar->row - 2 - rowOffset) * GAME_COLS))
                               : (caterpillarPos - ((caterpillar->row - 2 - rowOffset) * GAME_COLS));

        // todo:
        // Since this caterpillar is the only one
        // accesing its location, i don't think I need
        // to have a mutex for it, but we do need
        // one for the console functions
        // APPENDING^^^ I THINK WE DO NEED A MUTEX FOR EACH
        // CATERPILLAR SINCE THE LENGTH OF A CATERPILLAR
        // MAY CHANGE IF A BULLET HITS IT

        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        // draw enemy head
        consoleDrawImage(caterpillar->row, caterpillar->col, headFrame, ENEMY_HEIGHT);

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        // Draw the body tiles for each unit length
        // of the caterpillar
        for (int j = 0; j <= caterpillar->length; j++)
        {
            // We calculate the current segment's row/col
            // independently of the head, as the current body
            // segment may be on a different row than the head!
            segmentPos = caterpillarPos - (ENEMY_HEIGHT * (j + 1));
            if (segmentPos <= 0)
                break;

            // Each frame will use a different animation
            // than its neighbour
            bodyFrame = ENEMY_BODY[(segmentPos & 1)];

            // Determine what row the segment is on IGNORING rowOffset (for now)
            segmentRow = (int)ceil((double)segmentPos / GAME_COLS) + 1;

            // If we are a different row than the head, then the body segment
            // is moving in a different direction and the row offset will be different
            if (segmentRow == caterpillar->row - rowOffset)
            {
                segmentRow += rowOffset;

                segmentCol = (caterpillar->movingLeft == 1)
                                 ? GAME_COLS - (segmentPos - ((segmentRow - 2 - rowOffset) * GAME_COLS))
                                 : (segmentPos - ((segmentRow - 2 - rowOffset) * GAME_COLS));
            }
            else
            {
                segmentRow += rowOffset - 1;

                segmentCol = (caterpillar->movingLeft == 1)
                                 ? (segmentPos - ((segmentRow - 2 - (rowOffset - 1)) * GAME_COLS))
                                 : GAME_COLS - (segmentPos - ((segmentRow - 2 - (rowOffset - 1)) * GAME_COLS));
            }

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            if (j < caterpillar->length)
            {
                // Draw caterpillar body segment
                consoleDrawImage(segmentRow, segmentCol, bodyFrame, ENEMY_HEIGHT);
            }
            else
            {
                // Clear if we reached the end of the caterpillar
                consoleClearImage(segmentRow, segmentCol, ENEMY_HEIGHT, strlen(bodyFrame[0]));
            }

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }
        }

        // When we reach the end of the row, flip
        // the caterpillar->movingLeft flag and increment
        // the row offset
        if (caterpillarPos % GAME_COLS == 0)
        {
            caterpillar->movingLeft = !caterpillar->movingLeft;
            rowOffset++;
        }

        caterpillarPos++;

        // If we have waited nCyclesPerBullet,
        // then fire a bullet from the caterpillar's head
        if (bulletCounter-- == 0)
        {
            if (!fireBullet(caterpillar->col, caterpillar->row + ENEMY_HEIGHT, 0))
            {
                // caterpillar couldn't fire bullet
            }

            bulletCounter = nCyclesPerBullet;
        }

        // The closer the caterpillar is to the player, the
        // faster it moves.
        sleepTicks((caterpillar->sleeptTicks - rowOffset));
    }

    errorCode = pthread_mutex_lock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        pthread_exit(NULL);
    }

    // Clear the entire caterpillar animation
    consoleClearImage(caterpillar->row,
                      caterpillar->col,
                      ENEMY_HEIGHT,
                      strlen(bodyFrame[0]) * caterpillar->length);

    errorCode = pthread_mutex_unlock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        pthread_exit(NULL);
    }

    // Set flag based on if caterpillar made it to the player
    enemyAtBottomFlag = (caterpillar->row >= 16) ? 1 : 0;

    pthread_exit(NULL);
}

// Function that polls all currently alive
// caterpillars to determine if a particular caterpillar
// has been hit with a player bullet. If this is true, then we
// must split that caterpillar at the position it was hit.
// Depending on whether or not the split caterpillar's length
// is larger than ENEMY_MIN_LENGTH, we create a new thread for the
// half of the caterpillar that the original half was split from.
//
// todo:
//      There is an issue where if you hit the half of the caterpillar
//      that has yet to wrap around to the same row as the caterpillar's head
//      this function breaks down.
//
// Returns 1 if a caterpillar was hit, 0 otherwise.
int isCaterpillarHit(int row, int col)
{
    // given the row and col
    // determine if this hits a caterpillar,
    // and if it does then split the caterpillar
    EnemyNode *current = enemyHead;
    Caterpillar *enemy = NULL;

    int errorCode = 0;
    int newLength = 0;
    int newEnemyLength = 0;

    while (current != NULL)
    {
        enemy = current->enemy;

        current = current->next;

        // Don't bother checking dead enemies.
        if (enemy->length < ENEMY_MIN_LENGTH)
            continue;

        if (row < enemy->row + ENEMY_HEIGHT)
        {
            // We are handling things differently based
            // on which direciton the caterpllar is moving.
            // This is kind of annoying, refactor this if possible.
            if (enemy->movingLeft == 1)
            {
                if (col >= enemy->col && col <= enemy->col + (2 * enemy->length) + 1)
                {
                    // We have hit an enemy
                    // Cut down the current caterpillar
                    newLength = floor((col - (enemy->col + 1)) / 2);
                    newEnemyLength = enemy->length - newLength;
                    enemy->length = newLength;

                    if (newEnemyLength >= ENEMY_MIN_LENGTH)
                    {
                        // Spawn the new caterpillar from where the old one was hit
                        if (!spawnEnemy(col, enemy->row, newEnemyLength, enemy->movingLeft, enemy->sleeptTicks))
                            return 0;

                        // Launch animate thread for newly spawned enemy
                        errorCode = pthread_create(enemyHead->enemyThread, NULL, animateEnemy, (void *)enemyHead->enemy);
                        if (errorCode != 0)
                        {
                            print_error(errorCode, "pthread_create()");
                            return 0;
                        }
                    }
                    else
                    {
                        errorCode = pthread_mutex_lock(&M_Console);
                        if (errorCode != 0)
                        {
                            print_error(errorCode, "pthread_mutex_lock()");
                            pthread_exit(NULL);
                        }

                        // If the caterpillar does not meet the length requirement
                        // then we clear the caterpillar's prev animation frames
                        consoleClearImage(enemy->row, col, ENEMY_HEIGHT, strlen(ENEMY_BODY[0][0]) * newEnemyLength);

                        errorCode = pthread_mutex_unlock(&M_Console);
                        if (errorCode != 0)
                        {
                            print_error(errorCode, "pthread_mutex_unlock()");
                            pthread_exit(NULL);
                        }
                    }

                    // Subtract the number of ticks the enemy
                    // sleeps in between animations, speeding it up.
                    enemy->sleeptTicks -= 5;

                    return 1;
                }
            }
            else
            {
                if (col <= enemy->col && col >= enemy->col - (2 * enemy->length) - 1)
                {
                    // We have hit an enemy
                    // Cut down the current caterpillar
                    newLength = floor(((enemy->col - 1) - col) / 2);
                    newEnemyLength = enemy->length - newLength;
                    enemy->length = newLength;

                    if (newEnemyLength >= ENEMY_MIN_LENGTH)
                    {
                        // Spawn the new caterpillar from where the old one was hit
                        if (!spawnEnemy(col, enemy->row, newEnemyLength, enemy->movingLeft, enemy->sleeptTicks))
                            return 0;

                        // Launch animate thread for newly spawned enemy
                        errorCode = pthread_create(enemyHead->enemyThread, NULL, animateEnemy, (void *)enemyHead->enemy);
                        if (errorCode != 0)
                        {
                            print_error(errorCode, "pthread_create()");
                            return 0;
                        }
                    }
                    else
                    {
                        errorCode = pthread_mutex_lock(&M_Console);
                        if (errorCode != 0)
                        {
                            print_error(errorCode, "pthread_mutex_lock()");
                            pthread_exit(NULL);
                        }

                        // If the caterpillar does not meet the length requirement
                        // then we clear the caterpillar's prev animation frames
                        consoleClearImage(enemy->row, col, ENEMY_HEIGHT, strlen(ENEMY_BODY[0][0]) * newEnemyLength);

                        errorCode = pthread_mutex_unlock(&M_Console);
                        if (errorCode != 0)
                        {
                            print_error(errorCode, "pthread_mutex_unlock()");
                            pthread_exit(NULL);
                        }
                    }

                    // Subtract the number of ticks the enemy
                    // sleeps in between animations, speeding it up.
                    enemy->sleeptTicks -= 5;

                    return 1;
                }
            }
        }
    }

    return 0;
}

// Function that is cleans up all enemies
// currently stored in the linked list. This function
// frees all memory that was malloc'd earlier.
//
// Returns 1 indicating success, error otherwise.
int cleanupEnemies()
{
    int errorCode = 0;

    EnemyNode *current = enemyHead;
    EnemyNode *prev = NULL;

    while (current != NULL)
    {
        prev = current;
        current = current->next;

        // Join the thread that the caterpillar was running on
        errorCode = pthread_join(*prev->enemyThread, NULL);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_join()");
            pthread_exit(NULL);
        }

        errorCode = pthread_mutex_lock(&M_EnemyList);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        free(prev->enemy);
        free(prev->enemyThread);
        free(prev);

        errorCode = pthread_mutex_unlock(&M_EnemyList);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }
    }

    return 1;
}

// Function that spawns enemy caterpillars at the top
// right of the screen at a fixed interval set by ticksPerEnemy.
// This function will run in its own thread, and create a thread for
// each enemy caterpillar it spawns until the end of game.
//
// Runs until IS_RUNNING is false and exits with
// pthread_exit(NULL).
void *enemySpawner(void *ticksPerEnemy)
{
    // spawn enemies at given spawn rate
    int nTicksPerSpawn = *(int *)ticksPerEnemy;
    int spawnCountdown;
    int errorCode = 0;

    while (IS_RUNNING)
    {
        // Reset spawn countdown
        spawnCountdown = nTicksPerSpawn;

        // Spawn enemy at start location,
        // storing it at head of linked list.
        if (!spawnEnemy(GAME_COLS - 1, 2, ENEMY_DEFAULT_LENGTH, 1, ENEMY_DEFAULT_SPEED))
            pthread_exit(NULL);

        // Launch animate thread for newly spawned enemy
        errorCode = pthread_create(enemyHead->enemyThread, NULL, animateEnemy, (void *)enemyHead->enemy);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_create()");
            pthread_exit(NULL);
        }

        // Threads are joined in cleanup

        // Wait nTicksPerSpawn before spawning another enemy
        // This while loop will also not stall the thread
        // if the game dies while we are still waiting
        while (spawnCountdown-- >= 0 && IS_RUNNING)
            sleepTicks(1);
    }

    cleanupEnemies();

    pthread_exit(NULL);
}
