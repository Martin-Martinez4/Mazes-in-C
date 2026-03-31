#ifndef H_RAND_UTILS
#define H_RAND_UTILS

#include <stdlib.h>
#include <math.h>

int rand_triangle_distribution(double min, double max, double mode);
int rand_int(int min, int max);
float randomFloatInRange(float min, float max);
void shuffleArray(void* array, int length, size_t element_size);
int value_map_int(float value, float min_val, float max_val, int min, int max);

#endif