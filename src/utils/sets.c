#include <stdlib.h>
#include "sets.h"

// will have to free elements then Set
Set* createSet() {
  Set* s      = (Set*) malloc(sizeof(Set));
  s->elements = createRBTree();

  return s;
}

// add
// just insert to the RBTree
void add(Set* A, char* val) {
  rbTreeInsert(A->elements, val);
}

// remove
// have to get delete working in RBTree and use that
// void remove(Set* A, char* val){
//   printf("remove not implemented yet");
//   return;
// }

// has/find
// implement on RBTree
// traverse and try to find
bool has(Set* A, char* val) {
  return false;
}

char** getSetValues(Set* A) {
  char** strings = (char**) malloc(sizeof(char*) * A->elements->size);
  int index      = 0;
  traverseInOrder(A->elements, A->elements->root, strings, &index);
  return strings;
}

// merge (union is a C key word)
// get the values of both in a list
// in order traversal
// insert all the values in a loop
// Set* merge(Set* A, Set* B){

// }

// merge (union is a C key word)
// get the values of B
// add values to A
void mergeInPlace(Set* A, Set* B) {
  char** bValues = getSetValues(B);

  for (int i = 0; i < B->elements->size; i++) {
    add(A, bValues[i]);
  }
}

// Difference
// in A but not in B
// For all in A if in B do not add to new Set
// Set* difference(Set* A, Set* B);

// Intersection
// In A that are also in B
// For all in A if in B add to new Set
// Set* intersection(Set* A, Set* B);

// Symmetric difference
// In A or B exclusive (oh yeah xor)
// For all in A if in B add to new Set
// For all in B if in A add to new Set
// Set* symmetricDifference(Set* A, Set* B);