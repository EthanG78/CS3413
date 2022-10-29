#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

#include "bullet.h"
#include "globals.h"
#include "console.h"

char *PLAYER_BULLET = "^";

char *ENEMY_BULLET = "v";

// Define a bullet struct that tracks position
// of the bullet, as well as if it came from the
// player or from the enemy.
typedef struct Bullet
{
    int col;
    int row;
    int fromPlayer;
} Bullet;

// Define the node struct for our linked list of bullets.
// This node holds a reference to a bullet,
// a reference to a pthread_t where the bullet 'lives', and a
// reference to the next node in the list.
typedef struct BulletNode
{
    Bullet *bullet;
    pthread_t *bulletThread;
    struct BulletNode *next;
} BulletNode;

BulletNode *head = NULL;

int spawnBullet(int x, int y, int isFromPlayer)
{
    // Initialize a new bullet at col = x, row = y.
    Bullet *newBullet = (Bullet *)malloc(sizeof(Bullet));
    newBullet->col = x;
    newBullet->row = y;
    newBullet->fromPlayer = isFromPlayer;

    // Initialize the pthread that this new bullet will
    // execute on. We must allocate this on the stack.
    pthread_t *newBulletThread = (pthread_t *)malloc(sizeof(pthread_t));

    // We must store this caterpillar in the list of enemies.
    // It stores this new enemy at the front of the linked list
    BulletNode *newNode = (BulletNode *)malloc(sizeof(BulletNode));
    if (newNode == (BulletNode *)NULL)
    {
        perror("malloc()");
        return 0;
    }

    newNode->bullet = newBullet;
    newNode->bulletThread = newBulletThread;
    newNode->next = head;

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    head = newNode;

    errorCode = pthread_mutex_unlock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}