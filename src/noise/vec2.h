#ifndef H_NOISE_VEC2
#define H_NOISE_VEC2

typedef struct vec2 {
  float x;
  float y;
} vec2;

float dotVec2(vec2 v1, vec2 v2);
vec2 subtract2D(vec2 v1, vec2 v2);


#endif