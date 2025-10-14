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
  int size;
  struct RBNode* nilNode;
} RBTree;

RBTree *createRBTree();
RBNode *createRBNode(RBTree* tree, char* val);

void leftRotate(RBTree* tree, RBNode* pivotParent);
void rightRotate(RBTree* tree, RBNode* pivotParent);
void fixTree(RBTree* tree, RBNode* newNode);
void rbTreeInsert(RBTree* tree, char* string);
int traverseInOrder(RBTree *tree, RBNode *node, char **strings, int* index );
int traverseInOrderGetNodes(RBTree *tree, RBNode *node, RBNode** nodes, int* index);
bool searchRBTree(RBTree* tree, char* string);
void printTree(RBTree *tree, RBNode *node, int depth);
#endif