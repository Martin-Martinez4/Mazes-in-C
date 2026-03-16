#include "vec2.h"

float dotVec2(vec2 v1, vec2 v2){
  return v1.x*v2.x + v1.y*v2.y;
}
vec2 subtract2D(vec2 v1, vec2 v2){
  vec2 v = {.x = (v1.x - v2.x), .y = (v1.y - v2.y)};
  return v;
}