/*
  bullet.c
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

#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "globals.h"
#include "console.h"

// Flag that tells us to destroy all bullets currently running.
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

// Define the head of our bullet linked list.
BulletNode *bulletHead = NULL;

// This function spawns a new bullet at col x, and row y.
// We malloc this bullet and store it bullet in our linked
// list of bullets. We must pass isFromPlayer to the bullet
// struct as this affects the direction it travels and its animation.
//
// Returns the BulletNode that was stored on the linked list
// for this new bullet.  
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

// Function that is cleans up all bullets
// currently stored in the linked list. This function
// frees all memory that was malloc'd earlier.
//
// Returns 1 indicating success, error otherwise.
int cleanupBullets()
{
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

        // Join bullet thread
        errorCode = pthread_join(*prev->bulletThread, NULL);
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

        bulletHead = current;

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

// This function performs hit detection for a
// for a current bullet. Based on if the bullet was
// fired from a player or caterpillar, it handles things
// differently. 
//
// Returns 1 if a player or enemy was hit, and 0 otherwise.
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
        if (isCaterpillarHit(bullet->row, bullet->col))
            return 1;
    }

    return 0;
}

// Function that is responsible for animating the bullet
// fired from player or enemy. This is the function that is 
// running on each bullet thread. 
//
// Runs until IS_RUNNING is false, the bullet exits the bounds
// of the playable area, or the bullet has been signaled to be destroyed,
// and exits with pthread_exit(NULL).
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

        // Perform hit detection on each animation frame,
        // if we detect a hit on the player then we flip
        // IS_PLAYER_HIT, if we detect a hit on a caterpillar
        // then we increment the player's score.
        if (detectHit(bullet))
        {
            if (!bullet->fromPlayer)
            {
                // We have hit the player, signal the player
                // by setting the boolean value checked by player thread
                errorCode = pthread_mutex_lock(&M_IsPlayerHit);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_lock()");
                    return 0;
                }

                IS_PLAYER_HIT = 1;

                errorCode = pthread_mutex_unlock(&M_IsPlayerHit);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_unlock()");
                    return 0;
                }
            }
            else
            {
                errorCode = pthread_mutex_lock(&M_PlayerScore);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_lock()");
                    pthread_exit(NULL);
                }

                PLAYER_SCORE++;

                errorCode = pthread_mutex_unlock(&M_PlayerScore);
                if (errorCode != 0)
                {
                    print_error(errorCode, "pthread_mutex_unlock()");
                    pthread_exit(NULL);
                }
            }

            // break out of the loop when we hit a player or caterpillar
            break;
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

// Function that is responsible for 'firing' a bullet.
// This function will create a bulle twith the spawnBullet() function,
// and then launch a new thread for this bullet.
//
// Returns 1 indiciating success, error otherwise.
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

    return 1;
}
