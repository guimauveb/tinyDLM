#include "../include/BinaryTree.hxx"

#include <iostream>

BinaryTree::~BinaryTree() 
{
    destroyTree(root);
}

// Free memory allocated by the tree, going to the deepest node and deleting nodes moving back up.
void BinaryTree::destroyTree(Node *n)
{
    if (n != nullptr) {
        destroyTree(n->left);
        destroyTree(n->right);
        delete n;
    }
}

// Returns the height of the tree
int BinaryTree::height(Node *n)
{
    if (n == nullptr) {
        return 0;
    }
    return n->height;
}

int BinaryTree::getMaximumKey(Node *n)
{
    if (n == nullptr) {
        return INT_MIN;
    }
    return std::max(n->key, std::max(getMaximumKey(n->left), getMaximumKey(n->right)));
}

// Private members
int BinaryTree::getMinimumKey(Node *n)
{
    if (n == nullptr) {
        return INT_MAX;
    }
    return std::min(n->key, std::min(getMinimumKey(n->left), getMinimumKey(n->right)));
}

// Allocate a new node with a given key and no children (a leaf)
Node *BinaryTree::newNode(Node *n, int key)
{
    n = new Node;
    n->key = key;
    n->left = nullptr;
    n->right = nullptr;
    n->height = 1;

    return n;
}

// Right-rotate the subtree rooted with y
Node *BinaryTree::rightRotate(Node *y)
{
    Node *x = y->left;
    Node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = std::max(height(y->left),
            height(y->right)) + 1;
    x->height = std::max(height(x->left),
            height(x->right)) + 1;
    // Return new root
    return x;
}

Node *BinaryTree::leftRotate(Node *x)
{
    Node *y = x->right;
    Node *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = std::max(height(x->right),
            height(x->left)) + 1;

    y->height = std::max(height(y->right),
            height(y->left)) + 1;

    return y;
}

int BinaryTree::getBalance(Node *n)
{
    if (n == nullptr) {
        return 0;
    }
    return height(n->left) - height(n->right);
}

// Recursive function inserting a node in a tree rooted with "node". 
// Returns the new root of the subtree.
Node *BinaryTree::insert(Node* node, int key)
{
    /* 1. Perform the normal BST insertion. */
    if (key < node->key) {
        if (node->left != nullptr) {
            node->left = insert(node->left, key);
        } 
        else {
            node->left = newNode(node->left, key);
        }
    } 
    else if (key > node->key) {
        if (node->right != nullptr) {
            node->right = insert(node->right, key);
        } 
        else {
            node->right = newNode(node->right, key);
        }
    }
    // Equal keys are not allowed in BST.
    else {
        return node;
    }

    /* 2. Update node's height. */
    node->height = std::max(height(node->left),
            height(node->right)) + 1;

    /* 3. Get the balance factor of this ancestor node to check if it became unbalanced. */
    const int balance = getBalance(node);

    ///* If the tree is unbalanced, then we have 4 possibilities. */
    // Left left case
    if ((balance > 1) && (key < node->left->key)) {
        return rightRotate(node);
    }
    // Right right case
    if ((balance < -1) && (key > node->right->key)) {
        return leftRotate(node);
    }
    // Left right case
    if (balance > 1 && key < node->left->key) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    // Right left case
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;

}

// Printing nodes from the smallest key to the greatest.
void BinaryTree::preOrder(Node *r)
{
    if (r != nullptr) {
        std::cout << r->key << '\n';
        preOrder(r->left);
        preOrder(r->right);
    }
}

// Public members
void BinaryTree::preOrder()
{
    return preOrder(root);
}

void BinaryTree::insert(int key)
{
    if (root != nullptr) {
        root = insert(root, key);
    }
    else {
        root = newNode(root, key);
    }
}

Node *BinaryTree::deleteNode(Node *n, int key)
{
    if (n == nullptr) {
        return n;
    } 
    if (key < n->key) {
        n->left = deleteNode(n->left, key);
    }
    else if (key > n->key) {
        n->right = deleteNode(n->right, key);
    }
    else {
        // Node with only one child or no child
        if ((n->left == nullptr) || (n->right == nullptr)) {
            Node *tmp = (n->left != nullptr) ? n->left : n->right;
            // No child case
            if (tmp == nullptr) {
                delete n;
                n = nullptr;
            }
            // One child case
            else {
                // Copy the content of the non empty child
                *n = *tmp;
                free(tmp);
            }
        }
        /* Node with two children: get the inorder successor
         * (smallest in the right subtree) */
        // TODO - focus on this one
        else {
            Node *tmp = getMinimumValueNode(n->right);
            // Copy the inorder successor key to this node.
            n->key = tmp->key;
            // Delete the inorder successor.
            n->right = deleteNode(n->right, tmp->key);
        }
    }

    /* 3. Get the balance factor of this ancestor node to check if it became unbalanced. */
    const int balance = getBalance(n);

    ///* If the tree is unbalanced, then we have 4 possibilities. */
    // Left left case
    if ((balance > 1) && (key < n->left->key)) {
        return rightRotate(n);
    }
    // Right right case
    if ((balance < -1) && (key > n->right->key)) {
        return leftRotate(n);
    }
    // Left right case
    if (balance > 1 && key < n->left->key) {
        n->left = leftRotate(n->left);
        return rightRotate(n);
    }
    // Right left case
    if (balance < -1 && key < n->right->key) {
        n->right = rightRotate(n->right);
        return leftRotate(n);
    }

    return n;
}

Node *BinaryTree::getMinimumValueNode(Node *n)
{
    Node *current = n;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

Node *BinaryTree::getMaximumValueNode(Node *n)
{
    Node *current = n;
    while (current->right != nullptr) {
        current = current->right;
    }
    return current;
}

void BinaryTree::deleteNode(int key)
{
    root = deleteNode(root, key);
    last_key_removed = key;
}

// TODO - Create duplicate routine
/* Example:
 *      "filename.rar" 
 *      A binary tree exists for this filename:
 *
 *      3
 *     / \
 *    2   4
 *    
 *    Meaning that the next number we should append to this filename is 1 (smallest non existing value > 0)
 *    On removing, save the last value removed in a private variable. The next insert will be this value if != 0
 *    or the highest + 1.
 *
 *    insertNewVal() 
 *    {
 *    if lastKeyRemoved != 0 -> newVal = lastKeyRemoved
 *    else newVal = max + 1
 *    }
 */
int BinaryTree::createDuplicate()
{
    // Value returned that will be appended to the duplicate filename
    int index = 0;
    if (last_key_removed != 0) {
        root = insert(root, last_key_removed);
        index = last_key_removed;
        // Reset last_key_removed 
        last_key_removed = 0;
    }
    // Insert a new value equal to max_key + 1
    else {
        index = getMaximumKey(root) + 1;
        root = insert(root, getMaximumKey(root) + 1);
    }
    return index;
}

