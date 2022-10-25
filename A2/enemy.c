/*#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>*/

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
char* ENEMY_HEAD_LEFT[ENEMY_HEAD_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"o|",
   "  "},
  {"-|",
   "  "}
}

char* ENEMY_HEAD_RIGHT[ENEMY_HEAD_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"|o",
   "  "},
  {"|-",
   "  "}
}

// There are two animation frames for each body segment of the
// caterpillar. Each body segment has height two and width two. 
char* ENEMY_BODY[ENEMY_BODY_ANIM_TILES][ENEMY_HEIGHT] =
{
  {"~~",
   "/,"},
  {"~~",
   ",\\"}
};

int spawnEnemy(int x, int y)
{

}