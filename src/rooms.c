#include "rooms.h"

#include <stdio.h>

#include "rand_utils.h"

void append_room(Rooms* rooms, Room room) {
  if (rooms->length >= rooms->capacity) {
    rooms->capacity *= 2;
    Room* d = realloc(rooms->data, rooms->capacity * sizeof(Room));
    if (!d) {
      // handle allocation failure
      fprintf(stderr, "Failed to realloc rooms array\n");
      exit(1);
    }
    rooms->data = d;
  }
  // printf("Appending room %d / %d\n", rooms->length, rooms->capacity);
  rooms->data[rooms->length] = room;
  rooms->length += 1;
}

Rooms* create_rooms() {
  Room* d      = malloc(sizeof(Room) * 16);
  Rooms* rs    = malloc(sizeof(Rooms));
  rs->data     = d;
  rs->capacity = 16;
  rs->length   = 0;

  return rs;
}

int compare_rooms_by_x(const void* a, const void* b) {
  const Room* ra = (const Room*) a;
  const Room* rb = (const Room*) b;

  int ca = ra->aabb.x + ra->aabb.width / 2;
  int cb = rb->aabb.x + rb->aabb.width / 2;

  return ca - cb;
}

int compare_rooms_by_y(const void* a, const void* b) {
  const Room* ra = (const Room*) a;
  const Room* rb = (const Room*) b;

  int ca = ra->aabb.y + ra->aabb.width / 2;
  int cb = rb->aabb.y + rb->aabb.width / 2;

  return ca - cb;
}

void create_bvh(MazeStats* mazeStats, BVHNodes* nodes, Rooms* rooms) {
  // find longest side of grid

  // allocate and fill indices
  int* indices = malloc(sizeof(int) * rooms->length);
  for (int i = 0; i < rooms->length; i++) {
    indices[i] = i;
  }

  // assume split on width
  if (mazeStats->columns <= mazeStats->rows) {
    qsort(rooms->data, rooms->length, sizeof(Room), compare_rooms_by_x);
  } else {
    qsort(rooms->data, rooms->length, sizeof(Room), compare_rooms_by_y);
  }

  build_bvh(nodes, rooms, indices, 0, rooms->length);

  free(indices);
}

bool bvh_intersects_rooms(BVHNode* nodes, Room* rooms, int nodeIndex, Room newRoom) {
  // get current node
  BVHNode* node = &nodes[nodeIndex];

  if (!aabb_collides(node->box, newRoom.aabb)) {
    return false;
  }

  if (node->room_index >= 0) {
    return aabb_collides(node->box, rooms[node->room_index].aabb);
  }

  return bvh_intersects_rooms(nodes, rooms, node->left, newRoom) ||
         bvh_intersects_rooms(nodes, rooms, node->right, newRoom);
}

int bvh_insert(BVHNodes* nodes, Rooms* rooms, int nodeIndex, int roomIndex) {
  BVHNode* node = &nodes->data[nodeIndex];
  Room* room    = &rooms->data[roomIndex];

  // Leaf node → create a new internal node
  if (node->room_index != -1) {
    BVHNode oldLeaf = *node;

    BVHNode newLeaf = {.box = room->aabb, .left = -1, .right = -1, .room_index = roomIndex};

    append_bvh_node(nodes, newLeaf);

    int newLeafIndex = nodes->length - 1;

    node->left       = nodeIndex;
    node->right      = newLeafIndex;
    node->room_index = -1;
    node->box        = aabb_union(oldLeaf.box, newLeaf.box);

    return nodeIndex;
  }

  double expandLeft  = aabb_expansion_cost(nodes->data[node->left].box, room->aabb);
  double expandRight = aabb_expansion_cost(nodes->data[node->right].box, room->aabb);

  if (expandLeft < expandRight) {
    bvh_insert(nodes, rooms, node->left, roomIndex);
  } else {
    bvh_insert(nodes, rooms, node->right, roomIndex);
  }

  // Update bounding box
  node->box = aabb_union(nodes->data[node->left].box, nodes->data[node->right].box);
  return nodeIndex;
}

/*
  create rooms

  -
*/
Rooms* makeRooms(MazeStats* mazeStats, double saturation) {
  int columns = mazeStats->columns;
  int rows    = mazeStats->rows;

  double avg_cells_in_room = 16;
  int total_cells          = rows * columns;

  int max_rooms = (total_cells * saturation) / avg_cells_in_room;

  double current_saturation = 0;

  Rooms* rooms    = create_rooms();
  BVHNodes* nodes = create_bvh_nodes();

  while (current_saturation < saturation && rooms->length < max_rooms) {
    int room_width  = rand_triangle_distribution(2, 10, 5);
    int room_height = rand_triangle_distribution(2, 10, 5);

    int room_x = rand_int(0, columns - room_width);
    int room_y = rand_int(0, rows - room_height);

    AABB aabb = {.x = room_x, .y = room_y, .width = room_width, .height = room_height};

    Room r = {.aabb = aabb};

    bool collided = false;
    if (rooms->length >= 50) {
      // rebuild every 50 rooms
      if (rooms->length % 50 == 0) {
        // Reset before rebuild
        nodes->length = 0;
        create_bvh(mazeStats, nodes, rooms);
      }
      // root should be 0
      collided = bvh_intersects_rooms(nodes->data, rooms->data, 0, r);
    } else {
      // go throught each node and check for collision
      for (int i = 0; i < rooms->length; i++) {
        if (aabb_collides(r.aabb, rooms->data[i].aabb)) {
          collided = true;
          break;
        }
      }
    }
    if (!collided) {
      append_room(rooms, r);
      current_saturation = (rooms->length * avg_cells_in_room) / total_cells;
    }
  }

  return rooms;
}