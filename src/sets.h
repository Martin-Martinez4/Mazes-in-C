#ifndef H_SETS
#define H_SETS

#include "rbTree.h"

typedef struct Set{
  RBTree* elements;
} Set;


// add
// just insert to the RBTree
void add(Set* A, char* val);

// remove
// have to get delete working in RBTree and use that
// void remove(Set* A, char* val);

// has/find
// implement on RBTree
// traverse and try to find
bool has(Set* A, char* val);

// merge (union is a C key word)
// get the values of both in a list 
// in order traversal
// insert all the values in a loop
// Set* merge(Set* A, Set* B);

// merge (union is a C key word)
// get the values of B
// add values to A
void mergeInPlace(Set* A, Set* B);

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


#endif