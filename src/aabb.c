#include "aabb.h"

AABB compute_AABB(int min_x, int max_x, int min_y, int max_y) {
  AABB aabb = {
      .x      = min_x,
      .y      = min_y,
      .width  = max_x - min_x,
      .height = max_y - min_y,
  };

  return aabb;
}

AABB aabb_union(AABB aabb1, AABB aabb2) {
  int min_x = aabb1.x < aabb2.x ? aabb1.x : aabb2.x;
  int max_x = ((aabb1.x + aabb1.width) > (aabb2.x + aabb2.width)) ? (aabb1.x + aabb1.width)
                                                                  : (aabb2.x + aabb2.width);

  int min_y = aabb1.y < aabb2.y ? aabb1.y : aabb2.y;
  int max_y = ((aabb1.y + aabb1.height) > (aabb2.y + aabb2.height)) ? (aabb1.y + aabb1.height)
                                                                    : (aabb2.y + aabb2.height);

  return compute_AABB(min_x, max_x, min_y, max_y);
}

double aabb_expansion_cost(AABB a, AABB b) {
  AABB u = aabb_union(a, b);
  return (u.width * u.height) - (a.width * a.height);
}

bool aabb_collides_buffer(AABB a, AABB b, int buffer) {
    return !(a.x + a.width - 1 + buffer < b.x || b.x + b.width - 1 + buffer < a.x ||
             a.y + a.height - 1 + buffer < b.y || b.y + b.height - 1 + buffer < a.y);
}

bool aabb_collides(AABB a, AABB b) {
  return !(a.x + a.width - 1 < b.x || b.x + b.width - 1 < a.x || a.y + a.height - 1 < b.y ||
           b.y + b.height - 1 < a.y);
}