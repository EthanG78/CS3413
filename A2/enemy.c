#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    // length equivalent to the width of the game board initially
    Caterpillar newEnemy = {x, y, GAME_COLS};

    // We must store this caterpillar in the list of enemies.
    // It stores this new enemy at the front of the linked list
    // todo: this may cause issue
    EnemyNode *newNode = (EnemyNode *)malloc(sizeof(EnemyNode));
    if (newNode == (EnemyNode *)NULL)
    {
        perror("malloc()");
        return 0;
    }

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    newNode->enemy = &newEnemy;
    newNode->next = head;

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

    free(current);

    errorCode = pthread_mutex_unlock(&M_EnemyList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

void *animateEnemy(void *enemy)
{
    int errorCode = 0;
    Caterpillar *caterpillar = (Caterpillar *)enemy;

    while (IS_RUNNING)
    {
        // todo: needs testing...
        // Loop animation until caterpillar reaches player
        while (caterpillar->row < 16)
        {
            // todo:
            // worry about just moving left right now,
            // but we will need to address the wrap around
            // and moving right
            char **headFrame = ENEMY_HEAD_LEFT[(caterpillar->col & 1)];

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

                // TESTING:
                // this will draw items off screen, but I believe
                // that is what we want...
                errorCode = pthread_mutex_lock(&M_Console);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_lock()");
                    pthread_exit(NULL);
                }

                // draw body segment head width * j segments behind head
                consoleDrawImage(caterpillar->row, caterpillar->col + (2 * j), bodyFrame, ENEMY_HEIGHT);

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
            consoleClearImage(caterpillar->row, caterpillar->col + (2 * (caterpillar->length - 1)), ENEMY_HEIGHT, strlen(headFrame[0]));

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }

            // Move the caterpillar one column to the left (for now)
            caterpillar->col -= 1;
        }
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

void *enemyTest(void *idleTicks)
{
    int errorCode = 0;
    int nTicksPerAnimFrame = *(int *)idleTicks;
    /*pthread_t enemyThread;

    if (!initEnemies())
        pthread_exit(NULL);

    errorCode = pthread_create(&enemyThread, NULL, animateEnemy, head->enemy);
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
    cleanupEnemies();*/

    Caterpillar caterpillar = {GAME_COLS - 1, 2, GAME_COLS};

    while (IS_RUNNING)
    {
        // todo: needs testing...
        // Loop animation until caterpillar reaches player
        while (caterpillar.row < 16)
        {
            // todo:
            // worry about just moving left right now,
            // but we will need to address the wrap around
            // and moving right
            char **headFrame = ENEMY_HEAD_LEFT[(caterpillar.col & 1)];

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
            consoleDrawImage(caterpillar.row, caterpillar.col, headFrame, ENEMY_HEIGHT);

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }

            // Draw the body tiles for each unit length
            // of the caterpillar
            for (int j = 0; j < caterpillar.length; j++)
            {
                // Each frame will use a different animation
                // than its neighbour
                char **bodyFrame = ENEMY_BODY[(j & 1)];

                // TESTING:
                // this will draw items off screen, but I believe
                // that is what we want...
                errorCode = pthread_mutex_lock(&M_Console);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_lock()");
                    pthread_exit(NULL);
                }

                // draw body segment head width * j segments behind head
                consoleDrawImage(caterpillar.row, caterpillar.col + (2 * j), bodyFrame, ENEMY_HEIGHT);

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
            consoleClearImage(caterpillar.row, caterpillar.col + (2 * (caterpillar.length - 1)), ENEMY_HEIGHT, strlen(headFrame[0]));

            errorCode = pthread_mutex_unlock(&M_Console);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                pthread_exit(NULL);
            }
            // Move the caterpillar one column to the left (for now)
            caterpillar.col -= 1;

            // sleep nTicksPerAnimFrame * 20ms
            sleepTicks(nTicksPerAnimFrame);
        }
    }

    pthread_exit(NULL);
}