/*
  enemy.h
  CS3413 Operating Systems 1
  Assignment 2
  FALL 2022

  Author: Ethan Garnier
*/
#ifndef ENEMY_H
#define ENEMY_H

int enemiesRemaining();

int enemyAtBottom();

int cleanupEnemies();

int isCaterpillarHit(int row, int col);

void *enemySpawner(void *ticksPerEnemy);

#endif // ENEMY_H
