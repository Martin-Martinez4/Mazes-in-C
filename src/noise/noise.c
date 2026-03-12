#include "noise.h"
#include "grid_utils.h"
#include "stdlib.h"
#include "math.h"

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

float bilerpFromRowCol(int row, int col, float* scalePtr){

  float scale = (scalePtr) ? *scalePtr : 0.05f;

  float f00 = hash(row, col);
  float f10 = hash(row, col + 1);
  float f01 = hash(row + 1, col);
  float f11 = hash(row + 1, col + 1);


  float colScaled = col * (scale);
  float rowScaled = row * (scale);

  // x and y must be [0, 1]
  float x = colScaled - floor(colScaled);
  float y = rowScaled - floor(rowScaled);

  return bilerp(f00, f10, f01, f11, x, y);
}

float* applyNoise(int rows, int cols, float* scalePtr, NoiseFunc noiseFunc, void* params){

  float* noiseGrid = malloc(sizeof(float) * rows * cols);

  for(size_t row = 0; row < rows; row++){
    for(size_t col = 0; col < cols; col++){
      noiseGrid[matrix_coords_to_array_coords(row, col, cols)] = noiseFunc(row, col, scalePtr);
    }
  }
}
