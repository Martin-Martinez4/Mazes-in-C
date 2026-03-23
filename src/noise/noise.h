#ifndef H_NOISE_NOISE
#define H_NOISE_NOISE

#include "stdint.h"

typedef float (*NoiseFunc)(int x, int y, float* scalePtr);
// directly from the Simplex paper
// F2 skew
// G2 unskew
#define F2 0.5f * (sqrtf(3.0f) - 1.0f)
#define G2 (3.0f - sqrtf(3.0f)) / 6.0f

uint32_t hash(int x, int y);
uint32_t squirrel3(int x, int y, uint32_t seed);

float smoothStep(float t);

float lerp(float a, float b, float t);

float bilerpFromRowCol(int row, int col, float* scalePtr);
float perlinBilerp(int row, int col, float* scalePtr);
float simplexBilerp(int row, int col, float* scalePtr);

float* applyNoise(int rows, int cols, float* scalePtr, NoiseFunc noiseFunc, void* params);

#endif