#include "rb_tree.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "myAssert.h"

void Test_traverseInOrder(){

  RBTree *theTree = createRBTree();
  // maually create nodes to traverse

  // root 1
  RBNode *root1 = createRBNode(theTree, "C");

  RBNode *nodeD = createRBNode(theTree, "D");

  RBNode *nodeA = createRBNode(theTree, "A");
  RBNode *nodeB = createRBNode(theTree, "B");

  root1->right = nodeD;
  root1->left = nodeB;
  nodeB->left = nodeA;

  // root 2
  RBNode *root2 = createRBNode(theTree, "7");
  RBNode *node3 = createRBNode(theTree, "3"); 
  RBNode *node18 = createRBNode(theTree, "18");

  root2->left = node3;
  root2->right = node18;

  RBNode *node10 = createRBNode(theTree, "10");
  RBNode *node22 = createRBNode(theTree, "22");

  node18->left = node10;
  node18->right = node22;

  RBNode *node26 = createRBNode(theTree, "26");

  node22->right = node26;

  RBNode *node8 = createRBNode(theTree, "8");
  RBNode *node11 = createRBNode(theTree, "11");

  node10->left = node8;
  node10->right = node11;

  // root 3
  RBNode *root3 = createRBNode(theTree, "38");
  RBNode *node19 = createRBNode(theTree, "19");
  RBNode *node40 = createRBNode(theTree, "40");
  root3->left = node19;
  root3->right = node40;
  
  RBNode *node41 = createRBNode(theTree, "41");
  RBNode *node39 = createRBNode(theTree, "39");
  node40->left = node39;
  node40->right = node41;

  RBNode *node12 = createRBNode(theTree, "12");
  RBNode *node31 = createRBNode(theTree, "31");

  node19->left = node12;
  node19->right = node31;


  node8 = createRBNode(theTree, "8");
  node12->left = node8;

  typedef struct  testCase  {
    RBTree *theTree;
    RBNode *root; 
    char** want; 
    int length;

  }testCase;
  
  struct testCase *testCases = (struct testCase[]){ 
    {
      .theTree = theTree,
      .root = root1, 
      .want = (char *[]){"A", "B", "C", "D"},
      .length = 4

    }, 
    {
      .theTree = theTree,
      .root = root2,
      .want = (char *[]){"3", "7", "8", "10", "11", "18", "22", "26"},
      .length = 8
    },
    {
      .theTree = theTree,
      .root = root3,
      .want = (char *[]){"8", "12", "19", "31", "38", "39", "40", "41"},
      .length = 8
      
    }
    
  };

  int lenTestCases = 3;

  for(int i = 0; i < lenTestCases; i++){
    bool allPassed = true;
    printf("%d Running...\n", i);

    testCase tc = testCases[i];
    
    int index = 0;

    char** got = (char**)(malloc(tc.length * sizeof(char*)));
    int written = traverseInOrder(tc.theTree, tc.root, got, &index );
    
    if(written != tc.length){
      fprintf(stderr, "=====\n\033[31mAssertion failed: Test Case: %d, file %s, line %d. Message: length not the same written: %d, want: %d\033[0m\n=====\n", i,  __FILE__, __LINE__, written, tc.length); 

      written = written < tc.length ? written : tc.length;

      allPassed = false;
    }

    for(int j = 0; j < written; j++){
      if(strcmp(got[j], tc.want[j]) != 0){
        fprintf(stderr, "\t=====\n\033[31mAssertion failed: Test Case: %d, index: %d; got: %s, wanted: %s\033[0m\n=====\n",i,j, got[j], tc.want[j]); 
        allPassed = false;
      }
    }

    if(allPassed){
      fprintf(stderr, "=====\n\033[0;32mAll OK; %s, test case %d ,file %s, line %d\033[0m\n=====\n", "Test_traverseInOrder", i, __FILE__, __LINE__);
    }
  }
}

void Test_rbTreeInsert(){

  // RBTree *tree1 = createRBTree();
  // RBTree *tree2 = createRBTree();
  // RBTree *tree2 = createRBTree();

  // maually create nodes to traverse

  typedef struct  testCase  {
    RBTree *theTree;
    char** vals; 
    int length;
    char** wantVals;
    bool *wantIsRed;

  }testCase;
  
 struct testCase *testCases = (struct testCase[]){
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"5"},
    .length = 1,
    .wantVals = (char *[]){"5"},
    .wantIsRed = (bool []){false},
  }, 
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"5", "4"},
    .length = 2,
    .wantVals = (char *[]){"4", "5"},
    .wantIsRed = (bool []){true, false},
  }, 
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"5", "4", "3"},
    .length = 3,
    // "3" < "4" < "5"
    .wantVals = (char *[]){"3", "4", "5"},
    .wantIsRed = (bool []){true, false, true},
  }, 
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"10", "5", "1", "7", "6"},
    .length = 5,
    // Lexicographic order: "1", "10", "5", "6", "7"
    .wantVals = (char *[]){"1", "10", "5", "6", "7"},
    .wantIsRed = (bool []){false, false, true, false, true}, // approximate, color checks may differ slightly
  },
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"7", "3", "18", "10", "22", "8", "11", "26"},
    .length = 8,
    // Lexicographic order: "10", "11", "18", "22", "26", "3", "7", "8"
    // Because: "1" < "2" < "3" < "7" < "8"
    .wantVals = (char *[]){"10", "11", "18", "22", "26", "3", "7", "8"},
    .wantIsRed = (bool []){false, true, true, false, true, false, false, true},
  },
};

  int lenTestCases = 5;

  for(int i = 0; i < lenTestCases; i++){
    bool allPassed = true;
    printf("%d Running...\n", i);

    testCase tc = testCases[i];

    for(int n = 0; n < tc.length; n++){
      rbTreeInsert(tc.theTree, tc.vals[n]);
    }

    
    int index = 0;
    RBNode** got = (RBNode**)(malloc(tc.length * sizeof(RBNode*)));
    int written = traverseInOrderGetNodes(tc.theTree, tc.theTree->root, got, &index );
   
    
    if(written != tc.length){
      fprintf(stderr, "=====\n\033[31mAssertion failed: Test Case: %d, file %s, line %d. Message: length not the same written: %d, want: %d\033[0m\n=====\n", i,  __FILE__, __LINE__, written, tc.length); 

      written = written < tc.length ? written : tc.length;

      allPassed = false;
    }

    printf("tree size: %d\n", tc.theTree->size);

    for(int j = 0; j < written; j++){
      if(strcmp(got[j]->key, tc.wantVals[j]) != 0){
        fprintf(stderr, "\t=====\n\033[31mAssertion failed: Test Case: %d, index: %d; got: %s, wanted: %s\033[0m\n=====\n",i,j, got[j]->key, tc.wantVals[j]); 
        allPassed = false;
      }
      if(got[j]->red != tc.wantIsRed[j]){
        fprintf(stderr, "\t=====\n\033[31mAssertion failed: Test Case: %d, index: %d; got: %d, wanted: %d\033[0m\n=====\n",i,j, got[j]->red, tc.wantIsRed[j]); 
        allPassed = false;
      }
    }

    if(allPassed){
      fprintf(stderr, "=====\n\033[0;32mAll OK; %s, test case %d ,file %s, line %d\033[0m\n=====\n", "Test_traverseInOrder", i, __FILE__, __LINE__);
    }

    printTree(tc.theTree, tc.theTree->root, 0);
  }
}

void Test_searchRBTree(){

  // RBTree *tree1 = createRBTree();
  // RBTree *tree2 = createRBTree();
  // RBTree *tree2 = createRBTree();

  // maually create nodes to traverse

  typedef struct  testCase  {
    RBTree *theTree;
    char** vals; 
    int length;
    char** seachVals;
    bool *wants;
    int testLength;

  }testCase;
  
 struct testCase *testCases = (struct testCase[]){
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"5"},
    .length = 1,
    .seachVals = (char *[]){"5", "five", "4", "z"},
    .wants = (bool []){true, false, false, false},
    .testLength = 4
  }, 
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"5", "4"},
    .length = 2,
    .seachVals = (char *[]){"5", "five", "4", "z"},
    .wants = (bool []){true, false, true, false},
    .testLength = 4
  }, 
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"5", "4", "3"},
    .length = 3,
    // "3" < "4" < "5"
    .seachVals = (char *[]){"5", "five", "4", "3"},
    .wants = (bool []){true, false, true, true},
    .testLength = 4
  }, 
  {
    .theTree = createRBTree(),
    .vals = (char *[]){"10", "5", "1", "7", "6"},
    .length = 5,
    // Lexicographic order: "1", "10", "5", "6", "7"
    .seachVals = (char *[]){"10", "5", "1", "7", "6", "5", "five", "4", "3"},
    .wants = (bool []){true, true, true, true, true, true, false, false, false}, // approximate, color checks may differ slightly
    .testLength = 9
  }
};

  int lenTestCases = 4;

  for(int i = 0; i < lenTestCases; i++){
    bool allPassed = true;
    printf("%d Running...\n", i);

    testCase tc = testCases[i];

    for(int n = 0; n < tc.length; n++){
      rbTreeInsert(tc.theTree, tc.vals[n]);
    }

       
    for(int j = 0; j < tc.testLength; j++){
      bool got = searchRBTree(tc.theTree, tc.theTree->root, tc.seachVals[j]);
      if(got != tc.wants[j]){
        fprintf(stderr, "\t=====\n\033[31mAssertion failed: Test Case: %d, search #%d; Looking for: %s got: %d, wanted: %d\033[0m\n=====\n",i, j, tc.seachVals[j], got, tc.wants[j]); 
        allPassed = false;
      }
    }

    if(allPassed){
      fprintf(stderr, "=====\n\033[0;32mAll OK; %s, test case %d ,file %s, line %d\033[0m\n=====\n", "Test_traverseInOrder", i, __FILE__, __LINE__);
    }

    printTree(tc.theTree, tc.theTree->root, 0);
  }
}


void rbTreeRunTests(){
  Test_traverseInOrder();
  Test_rbTreeInsert();
  Test_searchRBTree();
}

