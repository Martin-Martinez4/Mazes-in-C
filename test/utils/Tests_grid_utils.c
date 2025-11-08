#include "myAssert.h"
#include "Tests_grid_utils.h"
#include "grid_utils.h"
#include <stdio.h>

void Test_coord_conversions() {

  typedef struct testCase {
    int x;
    int y;
    int columns;
    int want;
  } testCase;

  struct testCase* testCases = (struct testCase[]){{.x = 1, .y = 2, .columns = 5, .want = 7},
                                                   {.x = 1, .y = 2, .columns = 10, .want = 12},
                                                   {.x = 4, .y = 10, .columns = 20, .want = 90},
                                                   {.x = 0, .y = 10, .columns = 20, .want = 10}};

  int lenTestCases = 4;

  for (int i = 0; i < lenTestCases; i++) {
    printf("%d Running...", i);

    testCase tc = testCases[i];
    int got     = matrix_coords_to_array_coords(tc.x, tc.y, tc.columns);
    char buffer[256];
    sprintf(buffer, "matrix_coords_to_array_coords should return: %d got: %d", tc.want, got);
    MY_ASSERT(got == tc.want, buffer);
  }
}

void grid_utils_run_tests() {
  Test_coord_conversions();
}
