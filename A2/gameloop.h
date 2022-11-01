#ifndef GAMELOOP_H
#define GAMELOOP_H

// Initialize required data for game loop
// Return 1 indicating success, error otherwise
int initializeGameLoop();

// Begin the main game loop
void executeGameLoop();

// Cleanup data from game loop
// Return 1 indicating success, error otherwise
int cleanupGameLoop();

// Signals main thread that the game is over
// using the condition variable
int signalGameOver();

#endif // GAMELOOP_H