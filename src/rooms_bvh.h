#ifndef H_ROOMS_BVH
#define H_ROOMS_BVH

#include "aabb.h"

typedef struct Rooms Rooms;

typedef struct BVHNode {
  AABB box;
  int left;
  int right;
  int room_index;
} BVHNode;

typedef struct BVHNodes {
  BVHNode* data;
  int capacity;
  int length;
} BVHNodes;

BVHNodes* create_bvh_nodes();

void append_bvh_node(BVHNodes* BVHNodes, BVHNode node);

int build_bvh(BVHNodes* nodes, Rooms* rooms, int* indices, int start, int end);

#endif
