#include <string.h>
#include <stdio.h>
#include "sets.h"

void Test_mergeInPlace() {

  typedef struct testCase {
    Set* A;
    Set* B;
    char** aVals;
    int aLen;
    char** bVals;
    int bLen;
    char** wantVals;
    int wantLen;
  } testCase;

  struct testCase* testCases =
      (struct testCase[]){{// Case 0: disjoint sets
                           .A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){"1", "3", "5"},
                           .aLen     = 3,
                           .bVals    = (char*[]){"2", "4", "6"},
                           .bLen     = 3,
                           .wantVals = (char*[]){"1", "2", "3", "4", "5", "6"},
                           .wantLen  = 6},
                          {// Case 1: overlapping sets
                           .A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){"1", "3", "5"},
                           .aLen     = 3,
                           .bVals    = (char*[]){"3", "5", "7"},
                           .bLen     = 3,
                           .wantVals = (char*[]){"1", "3", "5", "7"},
                           .wantLen  = 4},
                          {// Case 2: empty B
                           .A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){"a", "b"},
                           .aLen     = 2,
                           .bVals    = (char*[]){},
                           .bLen     = 0,
                           .wantVals = (char*[]){"a", "b"},
                           .wantLen  = 2},
                          {// All elements overlap
                           .A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){"x", "y", "AA"},
                           .aLen     = 3,
                           .bVals    = (char*[]){"x", "y", "AA"},
                           .bLen     = 3,
                           .wantVals = (char*[]){"AA", "x", "y"},
                           .wantLen  = 3},
                          {.A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){"11", "12", "13"},
                           .aLen     = 3,
                           .bVals    = (char*[]){"x", "y", "z", "10"},
                           .bLen     = 4,
                           .wantVals = (char*[]){"10", "11", "12", "13", "x", "y", "z"},
                           .wantLen  = 7},
                          {// Case 3: empty A
                           .A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){},
                           .aLen     = 0,
                           .bVals    = (char*[]){"x", "y"},
                           .bLen     = 2,
                           .wantVals = (char*[]){"x", "y"},
                           .wantLen  = 2},
                          {// Case 4: both empty
                           .A        = createSet(),
                           .B        = createSet(),
                           .aVals    = (char*[]){},
                           .aLen     = 0,
                           .bVals    = (char*[]){},
                           .bLen     = 0,
                           .wantVals = (char*[]){},
                           .wantLen  = 0}};

  int lenTestCases = 7;

  for (int i = 0; i < lenTestCases; i++) {
    printf("%d Running...\n", i);
    testCase tc    = testCases[i];
    bool allPassed = true;

    // Populate A and B
    for (int n = 0; n < tc.aLen; n++)
      add(tc.A, tc.aVals[n]);
    for (int n = 0; n < tc.bLen; n++)
      add(tc.B, tc.bVals[n]);

    // Perform merge
    mergeInPlace(tc.A, tc.B);

    // Get merged values
    char** gotVals = getSetValues(tc.A);
    int gotLen     = tc.A->elements->size;

    if (gotLen != tc.wantLen) {
      printf("\t=====\n\033[31mAssertion failed: Test Case %d; size mismatch got: %d, want: "
             "%d\033[0m\n=====\n",
             i, gotLen, tc.wantLen);
      allPassed = false;
    } else {
      for (int j = 0; j < gotLen; j++) {
        if (strcmp(gotVals[j], tc.wantVals[j]) != 0) {
          printf("\t=====\n\033[31mAssertion failed: Test Case %d; index %d mismatch: got %s, want "
                 "%s\033[0m\n=====\n",
                 i, j, gotVals[j], tc.wantVals[j]);
          allPassed = false;
        }
      }
    }

    if (allPassed) {
      printf("=====\n\033[0;32mAll OK; %s, test case %d, file %s, line %d\033[0m\n=====\n",
             "Test_mergeInPlace", i, __FILE__, __LINE__);
    }

    // Optional visual check
    printf("Resulting set for case %d:\n", i);
    printTree(tc.A->elements, tc.A->elements->root, 0);
  }
}

void sets_run_tests() {
  Test_mergeInPlace();
}
