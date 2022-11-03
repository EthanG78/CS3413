#ifndef ENEMY_H
#define ENEMY_H

int enemiesRemaining();

int enemyAtBottom();

int isCaterpillarHit(int row, int col);

int cleanupEnemies();

void *enemySpawner(void *ticksPerEnemy);

#endif // ENEMY_H