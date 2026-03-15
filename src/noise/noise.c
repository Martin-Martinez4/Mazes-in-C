#include "noise.h"
#include "grid_utils.h"
#include "rand_utils.h"
#include "stdlib.h"
#include "math.h"
#include "stdio.h"
#include "vec2.h"

uint32_t hash(int x, int y) {
  uint32_t h = x;
  h ^= y * 0x27d4eb2d;
  h ^= h >> 15;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  return h;
}

uint32_t squirrel3(int x, int y, uint32_t seed) {
  uint32_t mangled = x;
  mangled *= 0x68E31DA4;
  mangled += y;
  mangled ^= seed;
  mangled *= 0xB5297A4D;
  mangled ^= mangled >> 8;
  mangled += 0x1B56C4E9;
  mangled ^= mangled << 8;
  mangled *= 0x85EBCA6B;
  return mangled;
}

float smoothStep(float t) {

  // return (t * t * 3.0) - (t * t * t * 2.0);
  return t * t * (3.0 - 2.0 * t);
}

float lerp(float a, float b, float t) {
  return a + t * (b - a);
}

float bilerp(float f00, float f10, float f01, float f11, float x, float y) {
  float a = lerp(f00, f10, x);
  float b = lerp(f01, f11, x);

  return lerp(a, b, y);
}

// Note to self:
// regions have more than one cell
// scale decides how many cells are in a region
// x0 and y0 are hashed because the is the region's x and y
float bilerpFromRowCol(int row, int col, float* scalePtr) {

  float scale = (scalePtr) ? *scalePtr : 0.05f;

  float colScaled = col * (scale);
  float rowScaled = row * (scale);
  int x0          = (int) floor(colScaled);
  int y0          = (int) floor(rowScaled);

  float x = colScaled - x0;
  float y = rowScaled - y0;

  // 0 to 1
  float f00 = (hash(x0, y0) / 4294967295.0f);
  float f10 = (hash(x0 + 1, y0) / 4294967295.0f);
  float f01 = (hash(x0, y0 + 1) / 4294967295.0f);
  float f11 = (hash(x0 + 1, y0 + 1) / 4294967295.0f);

  // x and y must be [0, 1]

  return bilerp(f00, f10, f01, f11, x, y);
}

vec2 gradient(int x, int y) {
  uint32_t h = hash(x, y);

  float angle = (h % 360) * (3.14159265f / 180.0f);

  vec2 g = {cosf(angle), sinf(angle)};
  return g;
}

float perlinBilerp(int row, int col, float* scalePtr) {

  float scale     = (scalePtr) ? *scalePtr : 0.05f;
  float colScaled = col * scale;
  float rowScaled = row * scale;

  int x0 = floor(colScaled);
  int y0 = floor(rowScaled);

  int x1 = x0 + 1;
  int y1 = y0 + 1;

  // fractional position inside cell
  float x = colScaled - x0;
  float y = rowScaled - y0;

  vec2 g00 = gradient(x0, y0);
  vec2 g10 = gradient(x1, y0);
  vec2 g01 = gradient(x0, y1);
  vec2 g11 = gradient(x1, y1);

  vec2 bottomLeft  = {.x = 0.f, .y = 0.f};
  vec2 bottomRight = {.x = 1.f, .y = 0.f};
  vec2 topLeft     = {.x = 0.f, .y = 1.f};
  vec2 topRight    = {.x = 1.f, .y = 1.f};

  vec2 d00 = {x, y};
  vec2 d10 = {x - 1, y};
  vec2 d01 = {x, y - 1};
  vec2 d11 = {x - 1, y - 1};

  return bilerp((dotVec2(g00, d00) + 1.f) / 2, (dotVec2(g10, d10) + 1.f) / 2,
                (dotVec2(g01, d01) + 1.f) / 2, (dotVec2(g11, d11) + 1.f) / 2, x, y);
}

float simplexBilerp(int row, int col, float* scalePtr) {
  float scale = (scalePtr) ? *scalePtr : 0.05f;

  float x = col * scale;
  float y = row * scale;

  float s = (x + y) * F2;

  // lattice coordinate i,j
  int i = floor(x + s);
  int j = floor(y + s);

  // unskew to get cell origin
  float t = (i + j) * G2;

  // unskew
  float x0 = i - t;
  float y0 = j - t;

  // distance from initial x and unskewed i (x0) nad j (y0)
  float dx0 = x - x0;
  float dy0 = y - y0;

  int i1, j1;
  if (dx0 > dy0) {
    i1 = 1;
    j1 = 0;
  } else {
    i1 = 0;
    j1 = 1;
  }

  // offsets at corners
  // dx1 is the distance from the input point to the second simplex corner
  // dx1 is the distance from the input point to the second simplex corner
  float dx1 = dx0 - i1 + G2;
  float dy1 = dy0 - j1 + G2;
  // The offset is larger because corner2 is further away in the lattice, and the
  // 2 * G2 term compensates for the extra skew correction needed for that larger move.
  float dx2 = dx0 - 1.0f + 2.0f * G2;
  float dy2 = dy0 - 1.0f + 2.0f * G2;

  vec2 g0 = gradient(i, j);
  vec2 g1 = gradient(i + i1, j + j1);
  vec2 g2 = gradient(i + 1, j + 1);

  float t0 = 0.5 - dx0 * dx0 - dy0 * dy0;
  float t1 = 0.5 - dx1 * dx1 - dy1 * dy1;
  float t2 = 0.5 - dx2 * dx2 - dy2 * dy2;

  float contrib0 = (t0 < 0 ? 0 : t0 *  dotVec2(g0, (vec2) {dx0, dy0}));
  float contrib1 = (t1 < 0 ? 0 : t1 *  dotVec2(g1, (vec2) {dx1, dy1}));
  float contrib2 = (t2 < 0 ? 0 : t2 *  dotVec2(g2, (vec2) {dx2, dy2}));
  float noise    = 5.0f * (contrib0 + contrib1 + contrib2);
  return (noise + 1.0f) * 0.5f;
}

float* applyNoise(int rows, int cols, float* scalePtr, NoiseFunc noiseFunc, void* params) {

  float* noiseGrid = malloc(sizeof(float) * rows * cols);

  for (size_t row = 0; row < rows; row++) {
    for (size_t col = 0; col < cols; col++) {
      noiseGrid[matrix_coords_to_array_coords(row, col, cols)] = noiseFunc(row, col, scalePtr);
    }
  }

  return noiseGrid;
}
