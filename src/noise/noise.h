#ifndef H_NOISE_NOISE
#define H_NOISE_NOISE

#include "stdint.h"

typedef float (*NoiseFunc)(float x, float y, void* params);

uint32_t hash(int x, int y);
uint32_t squirrel3(int x, int y, uint32_t seed);

float smoothStep(float t);

float lerp(float a, float b, float t);

float bilerp(float f00, float f10, float f01, float f11, float x, float y);
float bilerpFromRowCol(int row, int col, float* scalePtr);

float* applyNoise(int rows, int cols, float* scalePtr, NoiseFunc noiseFunc, void* params);

#endif