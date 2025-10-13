#include "rbTree.h"
#include "string.h"
#include <stdlib.h>

RBNode *createRBNode(RBTree* tree, char* val){
  RBNode* node = (RBNode *)malloc(sizeof(RBNode));

  node->key = val;
  node->parent = tree->nilNode;
  node->left = tree->nilNode;
  node->right = tree->nilNode;
  node->red = true;

  return node;

}

RBTree *createRBTree(){

  RBTree *rbt = (RBTree *)malloc(sizeof(RBTree));
  rbt->nilNode = NULL;
  rbt->depth = 0;
  rbt->root = NULL;
  rbt->size = 0;

  return rbt;
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

  if(pivotParent == tree->nilNode){
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

void rightRotate(RBTree* tree, RBNode* pivotParent){
  /* 
    parent becomes right child of current
    parent's left child becomes current's right child
    current left child stays
    parent right child stays

    if parent's parent is null make current root
  */

  if(pivotParent == tree->nilNode || pivotParent->left == tree->nilNode){
    return;
  }

  RBNode* pivot = pivotParent->left;
  pivotParent->left = pivot->right;


  if(pivot->right != tree->nilNode){
    pivot->right->parent = pivotParent;
  }

  pivot->parent = pivotParent->parent;
  if(pivotParent->parent == tree->nilNode){
    tree->root = pivot;
  }
  else if(pivotParent == pivotParent->parent->right){
    pivotParent->parent->right = pivot;
  }
  else{
    pivotParent->parent->left = pivot;
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
void fixTree(RBTree* tree, RBNode* newNode){

  RBNode * currentNode = newNode;

  while(tree->root != currentNode && currentNode->parent->red){
    RBNode *parent = currentNode->parent;
    RBNode *grandparent = parent->parent;

    if(parent == grandparent->right){
      RBNode* uncle = grandparent->left;

      if(uncle->red){
        uncle->red = false;
        parent->red = false;
        grandparent->red = true;
        currentNode = grandparent;
      }else{
        if(currentNode == parent->left){
          currentNode = parent;
          rightRotate(tree, currentNode);
          parent = currentNode->parent;
        }

        parent->red = false;
        grandparent->red = true;
        leftRotate(tree, grandparent);
      }
    }else{
      RBNode* uncle = grandparent->right;
      if(uncle->red){
        uncle->red = false;
        parent->red = false;
        grandparent->red = true;
        currentNode = grandparent;
      }else{
        if(currentNode == parent->right){
          currentNode = parent;
          leftRotate(tree, currentNode);
          parent = currentNode->parent;
        }

        parent->red = false;
        grandparent->red = true;
        rightRotate(tree, grandparent);
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

  RBNode *parent;
  RBNode *current = tree->root;

  while(current != tree->nilNode){
    parent = current;
    int cmp;

    if(current == tree->root){
      cmp = 1;
    }else{
      cmp = strcmp(node->key, current->key);
    }

    if(cmp > 0){
      current = current->right;
    }else if(cmp < 0){
      current = current->left;
    }else{
      // duplicate value
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
    return;
  }

  traverseInOrder(tree, node->left, strings, index);
  
  strings[*index] = node->key;

  traverseInOrder(tree, node->right, strings, index);

}

