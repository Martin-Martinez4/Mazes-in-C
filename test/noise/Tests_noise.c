#include "noise.h"
#include "stdbool.h"
#include "stdio.h"

void Test_noise() {
  typedef struct testCase {
    float a;
    float b;
    float t;
    float expected;

  } testCase;

  int numCases = 5;

  struct testCase* testCases = (struct testCase[]) {{1.f, 1.f, .25f, 1.f},
                                                    {-1.f, 1.f, .25f, -.5f},
                                                    {-1.f, -6.f, .25f, -2.25f},
                                                    {6.f, -6.f, .5f, 0.f},
                                                    {0.f, -6.f, .75f, -4.5f}};

  bool allPassed = true;
  for (int i = 0; i < numCases; i++) {
    printf("%d Running...\n", i);
    testCase tc = testCases[i];

    float got = lerp(tc.a, tc.b, tc.t);

    if (got != tc.expected) {
      printf("\t=====\n\033[31mAssertion failed: Test Case %d; got: %f, want: "
             "%f, file %s, line %d\033[0m\n=====\n",
             i, got, tc.expected, __FILE__, __LINE__);
    } else {
      printf("\t=====\n\033[32mOk: Test Case %d\033[0m\n=====\n", i);
    }
  }

  if (allPassed) {
    printf("=====\n\033[0;32mAll OK; simple lerp\033[0m\n=====\n");
  }
}

void noise_run_tests() {
  Test_noise();
}
