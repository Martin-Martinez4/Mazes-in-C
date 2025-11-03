#ifndef H_ROOMS
#define H_ROOMS

#include <stdbool.h>
#include "aabb.h"
#include "rooms_bvh.h"
#include "maze_stats.h"

// x is row #
// y is column #
// width and height units is cells

typedef struct Room {
  AABB aabb;
} Room;

typedef struct Rooms {
  Room* data;
  int capacity;
  int length;
} Rooms;

typedef struct BVHNodes BVHNodes;
typedef struct BVHNode BVHNode;

void append_room(Rooms* rooms, Room room);

Rooms* create_rooms();

int compare_rooms_by_x(const void* a, const void* b);

int compare_rooms_by_y(const void* a, const void* b);
void create_bvh(MazeStats* mazeStats, BVHNodes* nodes, Rooms* rooms);

bool bvh_intersects_rooms(BVHNode* nodes, Room* rooms, int nodeIndex,
                          Room newRoom);

Rooms* makeRooms(MazeStats* mazeStats, double saturation);

#endif