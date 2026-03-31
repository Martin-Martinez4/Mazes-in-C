#include "sets.h"
#include <stddef.h>

int find(int* sets, int x) {
  int root = x;

  // find true root
  while (sets[root] != root) {
    root = sets[root];
  }

  // compress root
  while (sets[x] != root) {
    int next = sets[x];
    sets[x]  = root;
    x        = next;
  }

  return root;
}

void mergeSets(int* sets, int current, int change) {
  int current_root = find(sets, current);
  int change_root  = find(sets, change);

  if (current_root != change_root) {
    sets[change_root] = current_root;
  }
}

void setUpSets(int* sets, Rooms* rooms, int rows, int columns) {
  for (int i = 0; i < rows * columns; i++) {
    sets[i] = i;
  }

  if (rooms == NULL) {
    return;
  }
  for (int i = 0; i < rooms->length; i++) {
    Room room = rooms->data[i];
    int set   = matrix_coords_to_array_coords(room.aabb.y, room.aabb.x, columns);

    int x      = room.aabb.x;
    int y      = room.aabb.y;
    int width  = room.aabb.width;
    int height = room.aabb.height;

    for (int row = y; row < (y + height - 1); row++) {
      for (int col = x; col < (x + width - 1); col++) {
        sets[matrix_coords_to_array_coords(row, col, columns)] = set;
      }
    }
  }
}
