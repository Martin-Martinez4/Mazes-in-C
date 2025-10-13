#ifndef H_RBTREE
#define H_RBTREE

#include <stdbool.h>

typedef struct RBNode {
  bool red;
  struct RBNode* left;
  struct RBNode* right;
  struct RBNode* parent;
  char* key;
} RBNode;

typedef struct RBTree {
  struct RBNode* root;
  int depth; 
  struct RBNode* nilNode;
} RBTree;

void leftRotate(RBTree* tree, RBNode* pivotParent);
void rightRotate(RBTree* tree, RBNode* pivotParent);
void fixTree(RBTree* tree, RBNode* newNode);
void rbTreeInsert(RBTree* tree, char* string);
#endif