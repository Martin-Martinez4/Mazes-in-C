#include "rand_utils.h"

int rand_triangle_distribution(double min, double max, double mode) {
  double u = (double) rand() / RAND_MAX;
  double value;

  if (u < (mode - min) / (max - min)) {
    value = min + sqrt(u * (max - min) * (mode - min));

  } else {
    value = max - sqrt((1 - u) * (max - min) * (max - mode));
  }

  int val = (int) (value + 0.5);
  if (val < min)
    val = min;
  if (val > max)
    val = max;
  return val;
}

int rand_int(int min, int max) {
  return min + rand() % (max - min + 1);
}

float randomFloatInRange(float min, float max){
  float random = (float)rand() / (float)RAND_MAX;
  float range = max -min;
  return (random * range) + min;
}

void shuffleArray(void* array, int length, size_t element_size) {
    if (length <= 1 || element_size == 0)
        return;

    unsigned char* arr = (unsigned char*) array;
    unsigned char* temp = (unsigned char*) malloc(element_size);
    if (!temp)
        return; // allocation failed

    for (int i = length - 1; i >= 1; i--) {
        int j = rand() % (i + 1);

        // swap arr[i] and arr[j]
        memcpy(temp, arr + i * element_size, element_size);
        memcpy(arr + i * element_size, arr + j * element_size, element_size);
        memcpy(arr + j * element_size, temp, element_size);
    }

    free(temp);
}

int value_map_int(float value, float min_val, float max_val, int min, int max) {
  if (value < min_val) {
    value = min_val;
  }
  if (value > max_val) {

    value = max_val;
  }

  float normalized = (value - min_val) / (max_val - min_val);

  int result = min + (int) (normalized * (max - min + 1));

  // clamp to max
  if (result > max) {

    result = max;
  }
  return result;
}
