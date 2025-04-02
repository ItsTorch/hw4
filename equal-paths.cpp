#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

// helper recursive fn. to go down the path to leaf nodes and count depth
bool check(Node* node, int depth, int& leafDepth, bool& firstLeaf) {
  // base case
  if (node == nullptr) return true;

  // if it is a leaf node
  if ((node->left == nullptr) && (node->right == nullptr)) {
    // only triggers by first leaf node hit to set initial depth
    if (!firstLeaf) {
      leafDepth = depth;
      firstLeaf = true;
      return true;
    }
    return depth == leafDepth;
  }
  // two recursive calls, to go down left and right subtree paths and check
  return check(node->left, depth + 1, leafDepth, firstLeaf) &&
         check(node->right, depth + 1, leafDepth, firstLeaf);
}

bool equalPaths(Node * root)
{
    // Add your code below
  // if tree is just the root itself
  if (root == nullptr) return true;

  int leafDepth = 0;
  bool firstLeaf = false;  
  return check(root, 0, leafDepth, firstLeaf);
}

