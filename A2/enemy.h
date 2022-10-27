#ifndef ENEMY_H
#define ENEMY_H

// Functions we need:
//  - Spawn caterpillar at top right of screren
//  - Handle caterpillar animation/movement

int enemiesRemaining();

int cleanupEnemies();

void *enemySpawner(void *ticksPerEnemy);

#endif // ENEMY_H