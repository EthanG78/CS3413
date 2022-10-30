#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

#include "bullet.h"
#include "globals.h"
#include "console.h"

char *PLAYER_BULLET[BULLET_ANIM_TILES][BULLET_HEIGHT] =
    {
        {"^"}};

char *ENEMY_BULLET[BULLET_ANIM_TILES][BULLET_HEIGHT] =
    {
        {"v"}};

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

BulletNode *bulletHead = NULL;

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
    newNode->next = bulletHead;

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    bulletHead = newNode;

    errorCode = pthread_mutex_unlock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

int destroyBullet(Bullet *bullet)
{
    BulletNode *current = bulletHead;
    BulletNode *prev = NULL;

    while (current->bullet != bullet)
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
    errorCode = pthread_mutex_lock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    // We must safely delete the desired
    // enemy, and re-establish continuity
    // in the enemy linked list
    if (current == bulletHead)
    {
        bulletHead = bulletHead->next;
    }
    else
    {
        prev->next = current->next;
    }

    // Join the thread that the bullet was running on
    errorCode = pthread_join(*current->bulletThread, NULL);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_join()");
        pthread_exit(NULL);
    }

    free(current->bullet);
    free(current->bulletThread);
    free(current);

    errorCode = pthread_mutex_unlock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

int cleanupBullets()
{
    // In the event that the game ends
    // and there are still bullets alive,
    // we need to free their memory and join their
    // threads.

    int errorCode = 0;

    BulletNode *current = bulletHead;
    BulletNode *prev = NULL;

    while (current != NULL)
    {
        prev = current;
        current = current->next;

        // Join the thread that the bullet was running on
        errorCode = pthread_join(*prev->bulletThread, NULL);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_join()");
            pthread_exit(NULL);
        }

        errorCode = pthread_mutex_lock(&M_BulletList);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        free(prev->bullet);
        free(prev->bulletThread);
        free(prev);

        errorCode = pthread_mutex_unlock(&M_BulletList);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }
    }

    return 1;
}

void *animateBullet(void *xBullet)
{
    int errorCode = 0;
    Bullet *bullet = (Bullet *)xBullet;

    char **bulletFrame = (bullet->fromPlayer == 1)
                             ? PLAYER_BULLET[0]
                             : ENEMY_BULLET[0];

    while (IS_RUNNING)
    {
        if (bullet->fromPlayer == 1 && bullet->row < 3)
        {
            // If the bullet was fired from the player,
            // then we want it to dissapear when we reach
            // the top of the screen.
            break;
        }

        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        // draw bullet
        consoleDrawImage(bullet->row, bullet->col, bulletFrame, BULLET_HEIGHT);

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        // If the bullet was fired from the player,
        // then we want the bullet to propagate
        // upwards, otherwise downwards.
        bullet->row = (bullet->fromPlayer == 1) ? bullet->row + 1 : bullet->row - 1;
    }

    // Cleanup the bullet
    destroyBullet(bullet);

    pthread_exit(NULL);
}

int fireBullet(int x, int y, int isFromPlayer)
{
    int errorCode = 0;

    // Spawn a new bullet object
    if (!spawnBullet(x, y, isFromPlayer))
    {
        return 0;
    }

    // Once successfull, animate the bullet
    errorCode = pthread_create(bulletHead->bulletThread, NULL, animateBullet, (void *)bulletHead->bullet);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_create()");
        return 0;
    }

    // Threads are joined in cleanup process

    return 1;
}