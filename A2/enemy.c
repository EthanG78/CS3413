#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

#include "enemy.h"
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

// Define a caterpillar struct that tracks the current row,
// the length of the caterpillar, and its current column.
// This is what we will store in linked list of all caterpillars
// on screen.
typedef struct Caterpillar
{
    int col;
    int row;
    int length;
} Caterpillar;

// Define the node struct for our linked list of enemies.
// This node holds a reference to a caterpillar and a
// reference to the next node in the list.
typedef struct EnemyNode
{
    Caterpillar *enemy;
    struct EnemyNode *next;
} EnemyNode;

EnemyNode *head;

int spawnEnemy(int x, int y)
{
    // Initialize a new caterpillar at the top right hand
    // corner of the game board. All new caterpillars have
    // length equivalent to the width of the game board initially.
    // Need to malloc each caterpillar so that it exists outside of this scope
    // and can be accessed as the linked list is accessed.
    Caterpillar *newEnemy = (Caterpillar *)malloc(sizeof(Caterpillar));
    newEnemy->col = x;
    newEnemy->row = y;
    newEnemy->length = GAME_COLS;

    // We must store this caterpillar in the list of enemies.
    // It stores this new enemy at the front of the linked list
    // todo: this may cause issue
    EnemyNode *newNode = (EnemyNode *)malloc(sizeof(EnemyNode));
    if (newNode == (EnemyNode *)NULL)
    {
        perror("malloc()");
        return 0;
    }

    newNode->enemy = newEnemy;
    newNode->next = head;

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    head = newNode;

    errorCode = pthread_mutex_unlock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

int initEnemies()
{
    // Initialize enemy caterpillars
    // by spawning first caterpillar
    // and allocating space for it
    // in linked list of enemies
    head = NULL;

    // Spawn first enemy, storing it at
    // head of linked list.
    if (!spawnEnemy(GAME_COLS - 1, 2))
        return 0;

    return 1;
}

int enemiesRemaining()
{
    // traverse the linked list, returning
    // the number of enemies currently on screen
    int length = 0;
    EnemyNode *current;

    for (current = head; current != NULL; current = current->next)
    {
        length++;
    }

    return length;
}

int destroyEnemy(Caterpillar *enemy)
{
    if (enemiesRemaining() == 0)
        return 1;

    // When a caterpillar reaches a length
    // less than ENEMY_MIN_LENGTH, we must destroy it.
    EnemyNode *current = head;
    EnemyNode *prev = NULL;

    while (current->enemy != enemy)
    {
        if (current->next == NULL)
        {
            // This should never happen, but if it
            // does then we have a mutex problem.
            return 0;
        }
        else
        {
            prev = current;
            current = current->next;
        }
    }

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    // We must safely delete the desired
    // enemy, and re-establish continuity
    // in the enemy linked list
    if (current == head)
    {
        head = head->next;
    }
    else
    {
        prev->next = current->next;
    }

    free(current->enemy);
    free(current);

    errorCode = pthread_mutex_unlock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

void *animateEnemy(void *node)
{
    int errorCode = 0;
    int nTicksPerAnimFrame = 25;
    int isGoingLeft = 1;
    int rowOffset = 0;
    Caterpillar *caterpillar = ((EnemyNode *)node)->enemy;

    // To determine where to draw each segment of the caterpillar,
    // I have come up with a clever way where we don't explicitly need to
    // increment/decrement rows/cols and care about the direction.
    // I number the caterpillar area as a grid as follows:
    //
    //  4  3  2  1
    //  5  6  7  8
    //  12 11 10 9          EXAMPLE 4x4 caterpillar grid
    //  13 14 15 16
    //
    // and store the caterpillar's head position (starts at 0)
    int caterpillarPos = 1;
    // int enemyRows = GAME_ROWS - 11;
    // int enemyCols = GAME_COLS;
    // int maxPos = enemyRows * enemyCols;

    // todo Revise:
    // I can calculate the caterpillars current row/col based
    // on the above grid using the following math:
    // row = ceil(pos / cols)
    // col = pos - ((row - 1) * enemyCols)
    // Based on the isGoingLeft flag, we subtract/add
    // the column number from the enemyCols variable

    // while (IS_RUNNING && caterpillar->row < 16)
    while (IS_RUNNING)
    {
        char **headFrame = (isGoingLeft == 1)
                               ? ENEMY_HEAD_LEFT[(caterpillar->col & 1)]
                               : ENEMY_HEAD_RIGHT[(caterpillar->col & 1)];

        // int row = (int)ceil(caterpillarPos / enemyRows);
        // int col = caterpillarPos - ((row - 1) * enemyCols);

        // Get the current row that the caterpillar is on
        caterpillar->row = (int)ceil((double)caterpillarPos / GAME_COLS) + 1 + rowOffset;

        // Based on which direction the caterpillar is facing, we either
        // subtract what column we are at from the max number of columns,
        // or just increment the columns
        caterpillar->col = (isGoingLeft == 1)
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
        for (int j = 0; j < caterpillar->length; j++)
        {
            // Each frame will use a different animation
            // than its neighbour
            char **bodyFrame = ENEMY_BODY[(j & 1)];

            // We calculate the current segment's row/col
            // independently of the head, as the current body
            // segment may be on a different row than the head!
            int segmentPos = caterpillarPos - (ENEMY_HEIGHT * (j + 1));
            if (segmentPos < 0)
                break;
            int segmentRow = (int)ceil((double)segmentPos / GAME_COLS) + 1 + rowOffset;
            int segmentCol = (isGoingLeft == 1)
                                 ? GAME_COLS - (segmentPos - ((segmentRow - 2 - rowOffset) * GAME_COLS))
                                 : (segmentPos - ((segmentRow - 2 - rowOffset) * GAME_COLS));

            // Draw body segment enemy head height/width * j segments
            // behind the head. This is an addition or subtraction
            // from caterpillar->col based on if it is going left or not
            // int segCol = (isGoingLeft == 1)
            //                 ? caterpillar->col + (ENEMY_HEIGHT * (j + 1))
            //                 : caterpillar->col - (ENEMY_HEIGHT * (j + 1));

            // We calculate the current segment row independently of

            errorCode = pthread_mutex_lock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_lock()");
                pthread_exit(NULL);
            }

            consoleDrawImage(segmentRow, segmentCol, bodyFrame, ENEMY_HEIGHT);

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }
        }

        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        // We need to clear the drawing directly behind the last
        // segment of the long caterpillar
        if (isGoingLeft == 1)
        {
            consoleClearImage(caterpillar->row, caterpillar->col + (2 * (caterpillar->length)), ENEMY_HEIGHT, strlen(headFrame[0]));
        }
        else
        {
            consoleClearImage(caterpillar->row, caterpillar->col - (2 * (caterpillar->length)), ENEMY_HEIGHT, strlen(headFrame[0]));
        }

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        // When we reach the end of the row, flip
        // the isGoingLeft flag and increment
        // the row offset
        if (caterpillarPos % GAME_COLS == 0)
        {
            isGoingLeft = !isGoingLeft;
            rowOffset++;
        }

        caterpillarPos++;

        // sleep nTicksPerAnimFrame * 20ms
        sleepTicks(nTicksPerAnimFrame);
    }

    pthread_exit(NULL);
}

int cleanupEnemies()
{
    // In the event that the game ends
    // and there are still caterpillars alive,
    // we need to free their memory.
    // todo:
    //      - we will also have to exit their threads...

    EnemyNode *current = head;
    EnemyNode *prev = NULL;

    while (current != NULL)
    {
        prev = current;
        current = current->next;

        int errorCode = 0;
        errorCode = pthread_mutex_lock(&M_EnemyList);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        free(prev->enemy);
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

void *enemyTest(void *x)
{
    int errorCode = 0;
    pthread_t enemyThread;

    if (!initEnemies())
        pthread_exit(NULL);

    errorCode = pthread_create(&enemyThread, NULL, animateEnemy, (void *)head);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_create()");
        pthread_exit(NULL);
    }

    errorCode = pthread_join(enemyThread, NULL);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_join()");
        pthread_exit(NULL);
    }

    // todo:
    // find optimal location for this
    // maybe in main??
    cleanupEnemies();

    pthread_exit(NULL);
}