#include "noise.h"

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

float smoothStep(float t){

  // return (t * t * 3.0) - (t * t * t * 2.0); 
  return t * t * (3.0 - 2.0 * t);
}

float lerp(float a, float b, float t){
  return a + t * (b - a);
}

float bilerp(float f00, float f10, float f01, float f11, float x, float y){
  float a = lerp(f00, f10, x);
  float b = lerp(f01, f11, x);

  return lerp(a, b, y);
}


