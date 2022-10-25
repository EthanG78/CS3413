#include <stdio.h>
#include <stdlib.h>
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
char *ENEMY_HEAD_LEFT[ENEMY_HEAD_ANIM_TILES][ENEMY_HEIGHT] =
    {
        {"o|",
         "  "},
        {"-|",
         "  "}};

char *ENEMY_HEAD_RIGHT[ENEMY_HEAD_ANIM_TILES][ENEMY_HEIGHT] =
    {
        {"|o",
         "  "},
        {"|-",
         "  "}};

// There are two animation frames for each body segment of the
// caterpillar. Each body segment has height two and width two.
char *ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] =
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
EnemyNode *current;

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

    newNode->enemy = &newEnemy;
    newNode->next = head;

    head = newNode;

    return 1;
}

int initEnemies()
{
    // Initialize enemy caterpillars
    // by spawning first caterpillar
    // and allocating space for it
    // in linked list of enemies
    head = NULL;
    current = head;

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

    for (current = head; current != NULL; current = current->next)
    {
        length++;
    }

    return length;
}