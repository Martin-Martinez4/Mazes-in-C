#ifndef H_MAZE_GEN_SETS
#define H_MAZE_GEN_SETS

#include "rooms.h"

int find(int* sets, int x);
void mergeSets(int* sets, int current, int change);
void setUpSets(int* sets, Rooms* rooms, int rows, int columns);

#endif