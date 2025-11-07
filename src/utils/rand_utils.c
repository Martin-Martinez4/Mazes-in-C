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
