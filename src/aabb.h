#ifndef H_AABB
#define H_AABB

#include <stdbool.h>

typedef struct AABB {
  int x;
  int y;
  int height;
  int width;
} AABB;

AABB compute_AABB(int min_x, int max_x, int min_y, int max_y);
AABB aabb_union(AABB aabb1, AABB aabb2);

double aabb_expansion_cost(AABB a, AABB b);
bool aabb_collides_buffer(AABB aabb1, AABB aabb2, int buffer);
bool aabb_collides(AABB aabb1, AABB aabb2);

#endif