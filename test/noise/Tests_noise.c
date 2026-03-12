#include "noise.h"

void Test_noise() {
typedef struct testCase {
    float a;
    float b;
    float t;

  } testCase;

  struct testCase* testCases = (struct testCase[]) {
    {1.f, 1.f, .25f}
  }

}

void noise_run_tests() {
  Test_noise();
}
