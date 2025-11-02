#ifndef H_ROOMS
#define H_ROOMS

#include "maze_stats.h"

typedef struct Room{
  int top_left_x;
  int top_left_y;
  int width;
  int height;
}Room;

Room *makeRooms(MazeStats* mazeStates);

#endif