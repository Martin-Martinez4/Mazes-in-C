#include "rb_tree.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>



RBNode *createRBNode(RBTree* tree, char* val){
  RBNode* node = (RBNode *)malloc(sizeof(RBNode));

  node->key = val;
  node->parent = tree->nilNode;
  node->left = tree->nilNode;
  node->right = tree->nilNode;
  node->red = true;

  return node;

}

RBTree* createRBTree() {
  RBTree* tree = malloc(sizeof(RBTree));
  tree->nilNode = malloc(sizeof(RBNode));

  tree->nilNode->red = false;
  tree->nilNode->left = tree->nilNode;
  tree->nilNode->right = tree->nilNode;
  tree->nilNode->parent = tree->nilNode;
  tree->nilNode->key = NULL;

  tree->root = tree->nilNode;
  tree->size = 0;

  return tree;
}

void leftRotate(RBTree* tree, RBNode* pivotParent){
  /*
    original node becomes left child of current node's right child
    left child of orignal stays
    right child of current's left child stays
    left child of current's left child becomes the current's right child

    Else if current is the left child of parent, make  current node's right child is left child of parent.
  */

  if(pivotParent == tree->nilNode || pivotParent->right == tree->nilNode){
    return;
  }

  RBNode* pivot = pivotParent->right;
  pivotParent->right = pivot->left;

  if(pivot->left != tree->nilNode){
    pivot->left->parent = pivotParent;
  }

  pivot->parent = pivotParent->parent;

  if(pivotParent->parent == tree->nilNode){
    tree->root = pivot;
  }
  else if(pivotParent == pivotParent->parent->left){
    pivotParent->parent->left = pivot;
  }
  else{
    pivotParent->parent->right = pivot;
  }

  pivot->left = pivotParent;
  pivotParent->parent = pivot;
}

void rightRotate(RBTree* tree, RBNode* pivotParent) {
    if (pivotParent == tree->nilNode || pivotParent->left == tree->nilNode) {
        return;
    }

    RBNode* pivot = pivotParent->left;
    pivotParent->left = pivot->right;

    if (pivot->right != tree->nilNode) {
        pivot->right->parent = pivotParent;
    }

    pivot->parent = pivotParent->parent;

    if (pivotParent->parent == tree->nilNode) {
        tree->root = pivot;
    } else if (pivotParent == pivotParent->parent->left) {
        pivotParent->parent->left = pivot;
    } else {
        pivotParent->parent->right = pivot;
    }

    pivot->right = pivotParent;
    pivotParent->parent = pivot;
}

/*
  Constraints:
    - root is always black
    - every leaf (nil) is black
    - Childern of red are always black
    - The simple path from any node to a leaf has to touch the same number of black nodes along the way.

  insert new node as red
  if that violates constraints recolor then rotate
*/
void fixTree(RBTree* tree, RBNode* newNode) {
  RBNode *currentNode = newNode;

  while (currentNode->parent->red) {
    RBNode *parent = currentNode->parent;
    RBNode *grandparent = parent->parent;

    if (parent == grandparent->left) {
      RBNode *uncle = grandparent->right;

      if (uncle->red) {
        parent->red = false;
        uncle->red = false;
        grandparent->red = true;
        currentNode = grandparent;
      } else {
        if (currentNode == parent->right) {
          currentNode = parent;
          leftRotate(tree, currentNode);
          parent = currentNode->parent;
        }
        parent->red = false;
        grandparent->red = true;
        rightRotate(tree, grandparent);
      }
    }
    else {
      RBNode *uncle = grandparent->left;

      if (uncle->red) {
        parent->red = false;
        uncle->red = false;
        grandparent->red = true;
        currentNode = grandparent;
      } else {
        if (currentNode == parent->left) {
            currentNode = parent;
            rightRotate(tree, currentNode);
            parent = currentNode->parent;
        }
        parent->red = false;
        grandparent->red = true;
        leftRotate(tree, grandparent);
      }
    }
  }

  tree->root->red = false;
}


void rbTreeInsert(RBTree* tree, char* string){
  // RBNode* node = (RBNode *)malloc(sizeof(RBNode));

  // node->key = string;
  // node->parent = tree->nilNode;
  // node->left = tree->nilNode;
  // node->right = tree->nilNode;
  // node->red = true;

  RBNode *node = createRBNode(tree, string);
  
  RBNode *parent = tree->nilNode;
  RBNode *current = tree->root;
  
  while(current != tree->nilNode){
    parent = current;
    int cmp = strcmp(node->key, current->key);
    
    if(cmp > 0){
      current = current->right;
    }else if(cmp < 0){
      current = current->left;
    }else{
      // duplicate value
      free(node);
      return;
    }
    
  }

  node->parent = parent;
  if(parent == tree->nilNode){
    tree->root = node;
  }else if(strcmp(node->key, parent->key) > 0){
    parent->right = node;
  }else{
    parent->left = node;
  }
    
  

  fixTree(tree, node);

  tree->size++;

}


int traverseInOrder(RBTree *tree, RBNode *node, char **strings, int* index ){
  if(node == tree->nilNode){
    return *index;
  }

  traverseInOrder(tree, node->left, strings, index);
  strings[*index] = node->key;
  (*index)++; 
  traverseInOrder(tree, node->right, strings, index);

  return *index;
}

int traverseInOrderGetNodes(RBTree *tree, RBNode *node, RBNode** nodes, int* index ){
  if(node == tree->nilNode){
    return *index;
  }

  traverseInOrderGetNodes(tree, node->left, nodes, index);
  nodes[*index] = node;
  (*index)++; 
  traverseInOrderGetNodes(tree, node->right, nodes, index);

  return *index;
}

bool searchRBTree(RBTree* tree, RBNode *node, char* string){
   if(node == tree->nilNode){
    return false;
  }
  
  int cmp = strcmp(string, node->key);

  if(cmp == 0){
    return true;
  }

  if(cmp > 0){

    return searchRBTree(tree, node->right, string);
  }else if(cmp < 0){
    return searchRBTree(tree, node->left, string);
  }
}


void printTree(RBTree *tree, RBNode *node, int depth) {
    if (node == tree->nilNode) return;

    printTree(tree, node->right, depth + 1);
    for (int i = 0; i < depth; i++) printf("    ");
    printf("%s (%s)\n", node->key, node->red ? "R" : "B");
    printTree(tree, node->left, depth + 1);
}

