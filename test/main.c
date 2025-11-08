#include <stdio.h>
#include <stdlib.h>
#include "tests.h"

int main() {
  rb_tree_run_tests();
  sets_run_tests();
  grid_utils_run_tests();
  printf("Program continues...\n");
  return 0;
}