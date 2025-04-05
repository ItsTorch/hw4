#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* node, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // tree is empty, create root
    if (this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    // insert at correct location
    while (current != nullptr) {
        if (new_item.first == current->getKey()) {
            // update value if key already exists
            current->setValue(new_item.second);
            return;
        }
        parent = current;
        current = (new_item.first < current->getKey()) ? 
                  current->getLeft() : current->getRight();
    }

    // create new node to link with parent
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    }
    else {
        parent->setRight(newNode);
    }
    
    // update balance, fix tree
    if (parent->getBalance() == 0) {
        parent->setBalance((newNode == parent->getLeft()) ? -1: 1);
        insertFix(parent, newNode);
    }
    else {
      parent->setBalance(0);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    // if node does not exist
    if (node == nullptr) return;

    // node being removed has two children
    if ((node->getLeft() != nullptr) && (node->getRight() != nullptr)) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred);
    }

    // get parent
    AVLNode<Key, Value>* parent = node->getParent();

    // find if node is left or right child
    int diff = 0;
    if (parent != nullptr) {
      diff = (parent->getLeft() == node) ? 1 : -1;
    }
    AVLNode<Key, Value>* child = node->getLeft() ? node->getLeft() : node->getRight();

    // if removing the root
    if (parent == nullptr) {
      this->root_ = child;
      if (child != nullptr) {
        child->setParent(nullptr);
      }
      delete node;
      return;
    }

    // connect parent to child
    if (parent->getLeft() == node) {
      parent->setLeft(child);
    }
    else {
      parent->setRight(child);
    }

    // connect child to parent if exists
    if (child != nullptr) {
      child->setParent(parent);
    }

    delete node;

    // rebalance if needed
    removeFix(parent, diff);
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

// rotate right helper function
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* y)
{
    AVLNode<Key, Value>* x = y->getLeft();
    AVLNode<Key, Value>* z = x->getRight();
    AVLNode<Key, Value>* parent = y->getParent();

    // rotation
    x->setRight(y);
    y->setLeft(z);

    // fix pointers to parents after rotation
    if (z != nullptr) z->setParent(y);
    y->setParent(x);

    // connect x to grandparent
    x->setParent(parent);

    // connecting y to grandparent after rotation
    if (parent == nullptr) this->root_ = x;
    else if (parent->getLeft() == y) parent->setLeft(x);
    else parent->setRight(x);
}

// rotate left helper function
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* x)
{
    AVLNode<Key, Value>* y = x->getRight();
    AVLNode<Key, Value>* z = y->getLeft();
    AVLNode<Key, Value>* parent = x->getParent();

    // rotation
    y->setLeft(x);
    x->setRight(z);

    // fix pointers to parents after rotation
    if(z != nullptr) z->setParent(x);
    x->setParent(y);

    // connect y to grandparent
    y->setParent(parent);

    // connecting y to grandparent after rotation
    if (parent == nullptr) this->root_ = y;
    else if (parent->getLeft() == x) parent->setLeft(y);
    else parent->setRight(y);
}

// insert helper function that checks balance and rebalances
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child)
{
    if (parent == nullptr || parent->getParent() == nullptr) return;

    AVLNode<Key, Value>* grandparent = parent->getParent();

    // changes grandparent's balance based if parent was inserted
    // insert left side
    if (parent == grandparent->getLeft()) {
        grandparent->updateBalance(-1);

        if (grandparent->getBalance() == 0) {
          return; // balanced
        }
        else if (grandparent->getBalance() == -1) {
          insertFix(grandparent, parent); // balanced, go up
        }
        else if (grandparent->getBalance() == -2) {
          //zig-zig
          if (child == parent->getLeft()) {
            rotateRight(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);
          }
          //zig-zag
          else {
            rotateLeft(parent);
            rotateRight(grandparent);

            if (child->getBalance() == -1) {
              parent->setBalance(0);
              grandparent->setBalance(1);
            }
            else if (child->getBalance() == 0) {
              parent->setBalance(0);
              grandparent->setBalance(0);
            }
            else {
              parent->setBalance(-1);
              grandparent->setBalance(0);
            }
            child->setBalance(0);
          }
        }
    }
    // right side
    else {
        grandparent->updateBalance(1);
        
        if (grandparent->getBalance() == 0) {
          return; // balanced
        }
        else if (grandparent->getBalance() == 1) {
          insertFix(grandparent, parent); // balanced, go up tree
        }
        else if (grandparent->getBalance() == 2) {
          // zig-zig
          if (child == parent->getRight()) {
            rotateLeft(grandparent);
            parent->setBalance(0);
            grandparent->setBalance(0);
          }
          // zig-zag
          else {
            rotateRight(parent);
            rotateLeft(grandparent);

            if (child->getBalance() == 1) {
              parent->setBalance(0);
              grandparent->setBalance(-1);
            }
            else if (child->getBalance() == 0) {
              parent->setBalance(0);
              grandparent->setBalance(0);
            }
            else {
              parent->setBalance(1);
              grandparent->setBalance(0);
            }
            child->setBalance(0);
          }
        }
    }
}

// remove helper function that checks balance and rebalances if needed
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    if (node == nullptr) return;

    AVLNode<Key, Value>* parent = node->getParent();
    int nextDiff = (parent != nullptr && parent->getLeft() == node) ? 1 : -1;

    // calc new balance
    int oldBalance = node->getBalance();
    int newBalance = oldBalance + diff;
    
    // case 1 - |newBalance| == 1 -> tree still balanced
    if (newBalance == 1 || newBalance == -1) {
        node->setBalance(newBalance);
        return;
    }
    // case 2 - newBalance == 0 -> tree is balanced
    else if (newBalance == 0) {
        node->setBalance(newBalance);
        if (parent != nullptr) {
            removeFix(parent, nextDiff);
        }
    }
    // case 3 - |newBalance| == 2 -> tree is unbalanced
    else if (newBalance == 2) {
        AVLNode<Key, Value>* rightChild = node->getRight();
        int rightChildBalance = rightChild->getBalance();
        
        // right-right
        if (rightChildBalance >= 0) {
            rotateLeft(node);
            if (rightChildBalance == 0) {
                node->setBalance(1);
                rightChild->setBalance(-1);
                return; // balanced
            } else {
                node->setBalance(0);
                rightChild->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        // right-left
        else {
            AVLNode<Key, Value>* rlChild = rightChild->getLeft();
            int rlBalance = rlChild->getBalance();
            rotateRight(rightChild);
            rotateLeft(node);
            
            if (rlBalance == 1) {
                node->setBalance(-1);
                rightChild->setBalance(0);
            } else if (rlBalance == 0) {
                node->setBalance(0);
                rightChild->setBalance(0);
            } else {
                node->setBalance(0);
                rightChild->setBalance(1);
            }
            rlChild->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
    else if (newBalance == -2) {
        AVLNode<Key, Value>* leftChild = node->getLeft();
        int leftChildBalance = leftChild->getBalance();
        
        // left-left
        if (leftChildBalance <= 0) {
            rotateRight(node);
            if (leftChildBalance == 0) {
                node->setBalance(-1);
                leftChild->setBalance(1);
                return; // balanced
            } else {
                node->setBalance(0);
                leftChild->setBalance(0);
                removeFix(parent, nextDiff);
            }
        }
        // left-right
        else {
            AVLNode<Key, Value>* lrChild = leftChild->getRight();
            int lrBalance = lrChild->getBalance();
            rotateLeft(leftChild);
            rotateRight(node);
            
            if (lrBalance == -1) {
                node->setBalance(1);
                leftChild->setBalance(0);
            } else if (lrBalance == 0) {
                node->setBalance(0);
                leftChild->setBalance(0);
            } else {
                node->setBalance(0);
                leftChild->setBalance(-1);
            }
            lrChild->setBalance(0);
            removeFix(parent, nextDiff);
        }
    }
}
#endif
