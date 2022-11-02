#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

#include "bullet.h"
#include "player.h"
#include "globals.h"
#include "console.h"

int destroyBullets = 0;

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

BulletNode *spawnBullet(int x, int y, int isFromPlayer)
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
        return NULL;
    }

    newNode->bullet = newBullet;
    newNode->bulletThread = newBulletThread;
    newNode->next = bulletHead;

    int errorCode = 0;
    errorCode = pthread_mutex_lock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return NULL;
    }

    bulletHead = newNode;

    errorCode = pthread_mutex_unlock(&M_BulletList);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return NULL;
    }

    return newNode;
}

int cleanupBullets()
{
    // In the event that the game ends
    // and there are still bullets alive, or
    // the player gets hit then we need to
    // free all bullet memory and join bullet
    // threads.
    int errorCode = 0;

    BulletNode *current = bulletHead;
    BulletNode *prev = NULL;

    errorCode = pthread_mutex_lock(&M_DestroyBullets);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    destroyBullets = 1;

    errorCode = pthread_mutex_unlock(&M_DestroyBullets);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    while (current != NULL)
    {
        prev = current;
        current = current->next;

        // Join the thread that the bullet was running on
        errorCode = pthread_join(*(prev->bulletThread), NULL);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_join()");
            return 0;
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

    errorCode = pthread_mutex_lock(&M_DestroyBullets);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        return 0;
    }

    destroyBullets = 0;

    errorCode = pthread_mutex_unlock(&M_DestroyBullets);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        return 0;
    }

    return 1;
}

int detectHit(Bullet *bullet)
{
    int errorCode = 0;
    if (!bullet->fromPlayer)
    {
        // Hit detection on player
        errorCode = pthread_mutex_lock(&M_PlayerPos);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            return 0;
        }

        if (bullet->row == PLAYER_POS_Y && bullet->col >= PLAYER_POS_X && bullet->col <= PLAYER_POS_X + PLAYER_WIDTH)
        {
            errorCode = pthread_mutex_unlock(&M_PlayerPos);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
                return 0;
            }
            return 1;
        }

        errorCode = pthread_mutex_unlock(&M_PlayerPos);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            return 0;
        }
    }
    else
    {
        // Hit detection on caterpillars
    }

    return 0;
}

void *animateBullet(void *xBullet)
{
    int errorCode = 0;
    Bullet *bullet = (Bullet *)xBullet;

    char **bulletFrame = (bullet->fromPlayer == 1)
                             ? PLAYER_BULLET[0]
                             : ENEMY_BULLET[0];

    // Bullet is alive if it is within the bounds of the playable
    // area, and the game is still running.
    while (IS_RUNNING && bullet->row > 3 && bullet->row < GAME_ROWS)
    {
        // Check if we called cleanupBullets
        errorCode = pthread_mutex_lock(&M_DestroyBullets);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        if (destroyBullets == 1)
        {
            errorCode = pthread_mutex_unlock(&M_DestroyBullets);
            if (errorCode != 0)
            {
                print_error(errorCode, "pthread_mutex_unlock()");
            }
            break;
        }

        errorCode = pthread_mutex_unlock(&M_DestroyBullets);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        errorCode = pthread_mutex_lock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_lock()");
            pthread_exit(NULL);
        }

        // clean last bullet pos
        consoleClearImage(bullet->row, bullet->col, BULLET_HEIGHT, strlen(bulletFrame[0]));

        errorCode = pthread_mutex_unlock(&M_Console);
        if (errorCode != 0)
        {
            print_error(errorCode, "pthread_mutex_unlock()");
            pthread_exit(NULL);
        }

        // If the bullet was fired from the player,
        // then we want the bullet to propagate
        // upwards, otherwise downwards. Reminder
        // that row - 1 will move the bullet upwards.
        bullet->row = (bullet->fromPlayer == 1) ? bullet->row - 1 : bullet->row + 1;

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

        if (detectHit(bullet))
        {
            if (!bullet->fromPlayer)
            {
                // We have hit the player
                playerHit();
            }
            else
            {
                // We have hit a caterpillar
            }
        }

        // Sleep 10 tick before moving bullet again
        sleepTicks(10);
    }

    errorCode = pthread_mutex_lock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_lock()");
        pthread_exit(NULL);
    }

    // clean last bullet pos
    consoleClearImage(bullet->row, bullet->col, BULLET_HEIGHT, strlen(bulletFrame[0]));

    errorCode = pthread_mutex_unlock(&M_Console);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_mutex_unlock()");
        pthread_exit(NULL);
    }

    pthread_exit(NULL);
}

int fireBullet(int x, int y, int isFromPlayer)
{
    int errorCode = 0;

    // Spawn a new bullet object
    BulletNode *bulletNode = spawnBullet(x, y, isFromPlayer);

    if (bulletNode == NULL)
        return 0;

    // Once successfull, animate the bullet
    errorCode = pthread_create(bulletNode->bulletThread, NULL, animateBullet, (void *)bulletNode->bullet);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_create()");
        return 0;
    }

    // todo:
    // THIS SUCKS BECAUSE WE WAIT UNTIL THE BULLET IS DONE FIRING,
    // WHAT I NEED TO DO IS CREATE A BULLET UPKEEP THREAD
    // THAT CONSTANTLY CHECKS ALL BULLETS AND JOINS THEM IN THE BACKGROUND
    // Join the thread that the bullet was running on

    // Joining and cleanup to an upkeep thread that is running in gameloop.c
    /*errorCode = pthread_join(*bulletNode->bulletThread, NULL);
    if (errorCode != 0)
    {
        print_error(errorCode, "pthread_join()");
        return 0;
    }*/

    return 1;
}