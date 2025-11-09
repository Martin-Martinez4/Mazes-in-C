#include "rooms.h"

#include <stdio.h>
#include <stdbool.h>

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

  int ca = ra->aabb.y + ra->aabb.height / 2;
  int cb = rb->aabb.y + rb->aabb.height / 2;

  return ca - cb;
}

void quicksort_indices(Rooms* rooms, int* indices, int left, int right, bool sortByY) {
    if (left >= right) return;

    int pivot = indices[right];
    int i = left;

    for (int j = left; j < right; j++) {
        int a = indices[j];
        int b = pivot;
        int va = sortByY ? (rooms->data[a].aabb.y + rooms->data[a].aabb.height/2)
                         : (rooms->data[a].aabb.x + rooms->data[a].aabb.width/2);
        int vb = sortByY ? (rooms->data[b].aabb.y + rooms->data[b].aabb.height/2)
                         : (rooms->data[b].aabb.x + rooms->data[b].aabb.width/2);
        if (va < vb) {
            int tmp = indices[i]; indices[i] = indices[j]; indices[j] = tmp;
            i++;
        }
    }
    int tmp = indices[i]; indices[i] = indices[right]; indices[right] = tmp;

    quicksort_indices(rooms, indices, left, i-1, sortByY);
    quicksort_indices(rooms, indices, i+1, right, sortByY);
}

void create_bvh(MazeStats* mazeStats, BVHNodes* nodes, Rooms* rooms) {
    int* indices = malloc(sizeof(int) * rooms->length);
    for (int i = 0; i < rooms->length; i++)
        indices[i] = i;

    // Sort indices, not the room array
    if (mazeStats->columns <= mazeStats->rows) {
        quicksort_indices(rooms, indices, 0, rooms->length -1, false);
    } else {
        quicksort_indices(rooms, indices, 0, rooms->length -1, true);
    }

    build_bvh(nodes, rooms, indices, 0, rooms->length);
    free(indices);
}

bool bvh_intersects_rooms(BVHNode* nodes, Room* rooms, int nodeIndex, Room newRoom) {
    if (nodeIndex < 0) return false; // safety check

    BVHNode* node = &nodes[nodeIndex];

    // If the new room doesn't intersect this node's bounding box, no need to check children
    if (!aabb_collides_buffer(node->box, newRoom.aabb, 1)) {
        return false;
    }

    // Leaf node → check actual room
    if (node->room_index >= 0) {
        return aabb_collides_buffer(newRoom.aabb, rooms[node->room_index].aabb, 1);
    }

    // Internal node → check children
    bool leftHit  = (node->left  != -1) ? bvh_intersects_rooms(nodes, rooms, node->left, newRoom)  : false;
    bool rightHit = (node->right != -1) ? bvh_intersects_rooms(nodes, rooms, node->right, newRoom) : false;
    return leftHit || rightHit;
}

int bvh_insert(BVHNodes* nodes, Rooms* rooms, int nodeIndex, int roomIndex) {
    BVHNode* node = &nodes->data[nodeIndex];
    Room* room    = &rooms->data[roomIndex];

    // If leaf, replace it with new internal node
    if (node->room_index != -1) {
        BVHNode oldLeaf = *node;

        // Create new leaf for the new room
        BVHNode newLeaf = {.box = room->aabb, .left = -1, .right = -1, .room_index = roomIndex};
        append_bvh_node(nodes, newLeaf);
        int newLeafIndex = nodes->length - 1;

        // Transform current leaf node into internal node
        node->left       = nodes->length; // allocate new internal node index below
        node->right      = newLeafIndex;
        node->room_index = -1;
        node->box        = aabb_union(oldLeaf.box, newLeaf.box);

        append_bvh_node(nodes, oldLeaf); // move old leaf to end
        node->left = nodes->length - 1; // point left to old leaf

        return nodeIndex;
    }

    // Choose which subtree expands less
    double expandLeft  = aabb_expansion_cost(nodes->data[node->left].box, room->aabb);
    double expandRight = aabb_expansion_cost(nodes->data[node->right].box, room->aabb);

    if (expandLeft < expandRight) {
        bvh_insert(nodes, rooms, node->left, roomIndex);
    } else {
        bvh_insert(nodes, rooms, node->right, roomIndex);
    }

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
  
  int attempts_allowed = rows * columns;
  while (current_saturation < saturation && rooms->length < max_rooms && attempts_allowed >= 0) {
    
    
    
      int min_room_width = 2;
      int max_room_width = 10;
    
      int min_room_height = 2;
      int max_room_height = 10;
    
      int attempts_per_room = 100;


    while(attempts_per_room >= 0 ){

      int room_width  = rand_triangle_distribution(min_room_width, max_room_width, (max_room_width + min_room_width)/2);
      int room_height = rand_triangle_distribution(min_room_height, max_room_height, (max_room_height + min_room_height)/2);
  
      int room_x = rand_int(0, columns - room_width);
      int room_y = rand_int(0, rows - room_height);
  
      AABB aabb = {.x = room_x, .y = room_y, .width = room_width, .height = room_height};
  
      Room r = {.aabb = aabb};
  
      bool collided = false;

        for (int i = 0; i < rooms->length; i++) {
          if (aabb_collides_buffer(r.aabb, rooms->data[i].aabb, 1)) {
            collided = true;
            break;
          }
        }
      // }
      if (!collided) {
        append_room(rooms, r);
        current_saturation = (rooms->length * avg_cells_in_room) / total_cells;
        break;
      }

      attempts_per_room--;

      if(attempts_per_room <= 7){

        max_room_width--;
        max_room_height--;
      }
    }

    attempts_allowed--;

  }

  printf("Actual Room Saturation Achieved: %f; Target Saturation: %f\n", current_saturation, saturation);
  printf("Number Rooms: %d\n", rooms->length);
  return rooms;
}