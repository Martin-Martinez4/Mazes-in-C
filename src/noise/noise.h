#ifndef H_NOISE_NOISE
#define H_NOISE_NOISE

#include "stdint.h"
#include "vec2.h"

typedef struct {
    float theta;
    float dir_x;
    float dir_y;
    float t_min;
    float t_max;
} LinearGradientParams;

typedef struct {
    int cx;
    int cy;
    float maxDist;
    float t_min;
    float t_max;
} RadialGradientParams;

typedef float (*NoiseFunc)(int x, int y, float* scalePtr, void* params);
// directly from the Simplex paper
// F2 skew
// G2 unskew
#define F2 0.5f * (sqrtf(3.0f) - 1.0f)
#define G2 (3.0f - sqrtf(3.0f)) / 6.0f
#define MY_PI 3.141592653589793

uint32_t hash(int x, int y);
uint32_t squirrel3(int x, int y, uint32_t seed);

float smoothStep(float t);

float lerp(float a, float b, float t);

float bilerpFromRowCol(int row, int col, float* scalePtr, void* params);
float perlinBilerp(int row, int col, float* scalePtr, void* params);
float simplexBilerp(int row, int col, float* scalePtr, void* params);

LinearGradientParams create_linear_gradient_params(int rows, int cols, float degrees);
float linear_gradient(int row, int col, float* scalePtr, void* params);

RadialGradientParams create_radial_gradient_params(int rows, int cols, int cx, int cy);
float radial_gradient(int row, int col, float* scalePtr, void* params);

float perlin_warped(int row, int col, float* scalePtr, void* params);

float* applyNoise(int rows, int cols, float* scalePtr, NoiseFunc noiseFunc, void* params);

#endif