#include <cstdint>
#ifndef H_NOISE_NOISE
#define H_NOISE_NOISE

uint32_t hash(int x, int y);
uint32_t squirrel3(int x, int y, uint32_t seed);

float smoothStep(float t);

float lerp(float a, float b, float t);

// f00 bottom-left hashed
// f10 bottom-right hashed
// f01 top-left hashed
// f11 top-right hashed
float bilerp(float f00, float f10, float f01, float f11, float x, float y);

// value noise is bilerp over a grid
#endif