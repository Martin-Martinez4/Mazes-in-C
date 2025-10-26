#include "draw_cells.h"
#include <stdio.h>
#include "myAssert.h"


void Test_drawCells(){

  typedef struct  testCase  {
    int x; 
    int y; 
    int columns;
    int want;
  }testCase;


  struct testCase *testCases = (struct testCase[]){ 
    {
      .x = 1, 
      .y = 2,
      .columns = 5,
      .want = 7
    }, 
    {
      .x = 1, 
      .y = 2,
      .columns = 10,
      .want = 12
    },
     {
      .x = 4, 
      .y = 10,
      .columns = 20,
      .want = 90
    },
    {
      .x = 0, 
      .y = 10,
      .columns = 20,
      .want = 10
    } 
  };

  int lenTestCases = 4;

  for(int i = 0; i < lenTestCases; i++){
   printf("%d Running...", i);

    testCase tc = testCases[i];
    int got = coordsMatrixToArray(tc.x, tc.y, tc.columns);
    char buffer[256];
    sprintf(buffer, "coordsMatrixToArray should return: %d got: %d" , tc.want, got);
    MY_ASSERT(got == tc.want, buffer);
   
  } 




}

