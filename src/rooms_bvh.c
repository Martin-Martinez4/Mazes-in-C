#include "rooms.h"
#include "aabb.h"
#include "rooms_bvh.h"
#include <stdio.h>
#include <stdlib.h>

BVHNodes* create_bvh_nodes() {
  BVHNode* d   = malloc(sizeof(BVHNode) * 16);
  BVHNodes* rs = malloc(sizeof(BVHNodes));
  rs->data     = d;
  rs->capacity = 16;
  rs->length   = 0;

  return rs;
}

void append_bvh_node(BVHNodes* BVHNodes, BVHNode node) {
  if (BVHNodes->length >= BVHNodes->capacity) {
    BVHNodes->capacity *= 2;
    BVHNode* d = realloc(BVHNodes->data, BVHNodes->capacity * sizeof(BVHNode));
    if (!d) {
      // handle allocation failure
      fprintf(stderr, "Failed to realloc BVHNodes array\n");
      exit(1);
    }
    BVHNodes->data = d;
  }

  BVHNodes->data[BVHNodes->length] = node;
  BVHNodes->length += 1;
}

int build_bvh(BVHNodes* nodes, Rooms* rooms, int* indices, int start, int end) {
  int count = end - start;
  if (count <= 0) {
    return -1;
  }

  // initialize min/max from the first room in the slice
  Room* r0  = &rooms->data[indices[start]];
  int min_x = r0->aabb.x;
  int min_y = r0->aabb.y;
  int max_x = r0->aabb.x + r0->aabb.width;
  int max_y = r0->aabb.y + r0->aabb.height;

  for (int i = start + 1; i < end; i++) {
    Room* ri = &rooms->data[indices[i]];
    int rx0  = ri->aabb.x;
    int ry0  = ri->aabb.y;
    int rx1  = ri->aabb.x + ri->aabb.width;
    int ry1  = ri->aabb.y + ri->aabb.height;

    if (rx0 < min_x)
      min_x = rx0;
    if (ry0 < min_y)
      min_y = ry0;
    if (rx1 > max_x)
      max_x = rx1;
    if (ry1 > max_y)
      max_y = ry1;
  }

  // leaf node append
  if (count == 1) {
    BVHNode leaf;
    leaf.box        = compute_AABB(min_x, max_x, min_y, max_y);
    leaf.left       = -1;
    leaf.right      = -1;
    leaf.room_index = indices[start];
    append_bvh_node(nodes, leaf);
    return nodes->length - 1;
  }

  bool splitY = (max_y - min_y) > (max_x - min_x);

  // Sort indices by center along split axis (simple insertion sort; fine for small arrays)
  for (int i = start + 1; i < end; i++) {
    int key     = indices[i];
    int key_val = splitY ? (rooms->data[key].aabb.y + rooms->data[key].aabb.height / 2)
                         : (rooms->data[key].aabb.x + rooms->data[key].aabb.width / 2);
    int j       = i - 1;
    while (j >= start) {
      int j_val = splitY
                      ? (rooms->data[indices[j]].aabb.y + rooms->data[indices[j]].aabb.height / 2)
                      : (rooms->data[indices[j]].aabb.x + rooms->data[indices[j]].aabb.width / 2);
      if (j_val <= key_val) {

        break;
      }
      indices[j + 1] = indices[j];
      j--;
    }
    indices[j + 1] = key;
  }

  // Split at median
  int mid         = start + count / 2;
  int left_index  = build_bvh(nodes, rooms, indices, start, mid);
  int right_index = build_bvh(nodes, rooms, indices, mid, end);

  // Internal node
  BVHNode box;
  box.left       = left_index;
  box.right      = right_index;
  box.room_index = -1;
  AABB left_box  = nodes->data[left_index].box;
  AABB right_box = nodes->data[right_index].box;
  box.box        = aabb_union(left_box, right_box);
  append_bvh_node(nodes, box);

  return nodes->length - 1;
}
