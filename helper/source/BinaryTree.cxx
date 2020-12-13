#include "../include/BinaryTree.hxx"

#include <iostream>

BinaryTree::BinaryTree()
{
    root = nullptr;
}

BinaryTree::~BinaryTree()
{
    destroyTree();
}

void BinaryTree::destroyTree(Node *leaf)
{
    if (leaf != nullptr) {
        destroyTree(leaf->left);
        destroyTree(leaf->right);
        delete leaf;
    }
}

void BinaryTree::insert(int key, Node *leaf)
{
    if (key < leaf->key_value) {
        if (leaf->left != nullptr) {
            insert(key, leaf->left);
        }
        else {
            leaf->left = new Node;
            leaf->left->key_value = key;
            leaf->left->left = nullptr;
            leaf->left->right = nullptr;
        }
    }
    else if (key >= leaf->key_value) {
        if (leaf->right != nullptr) {
            insert(key, leaf->right);
        }
        else {
            leaf->right = new Node;
            leaf->right->key_value = key;
            leaf->right->left = nullptr;
            leaf->right->right = nullptr;
        }
    }
}

Node *BinaryTree::search(int key, Node *leaf)
{
    if (leaf != nullptr) {
        if (key == leaf->key_value) {
            return leaf;
        }
        if (key < leaf->key_value) {
            return search(key, leaf->left);
        }
        else {
            return search(key, leaf->right);
        }
    }
    else return nullptr;
}

void BinaryTree::insert(int key)
{
    if (root != nullptr) {
        insert(key, root);
    }
    else {
        root = new Node;
        root->key_value = key;
        root->left = nullptr;
        root->right = nullptr;
    }
}

Node *BinaryTree::search(int key)
{
    return search(key, root);
}

/* Example:
 *      "filename.rar" 
 *      A binary tree exists for this filename:
 *
 *      2
 *     / \
 *    1   3
 *    
 *    Meaning that the next number we should append to this filename is 1 (smallest non existing value > 0)
 *    On removing, save the last value removed in a private variable. The next insert will be this value if != 0
 *    or the highest + 1.
 *
 *    insertNewVal() 
 *    {
 *    if lastValueRemoved != 0 -> newVal = lastValueRemoved
 *    else newVal = max + 1
 *    }
 */

// Private member
int BinaryTree::getMaximumKey(Node *leaf)
{
    if (leaf == nullptr) {
        return INT_MIN;
    }
    return std::max(leaf->key_value, std::max(getMaximumKey(leaf->left), getMaximumKey(leaf->right)));
}

// Private member
int BinaryTree::getMinimumKey(Node *leaf)
{
    if (leaf == nullptr) {
        return INT_MAX;
    }
    return std::min(leaf->key_value, std::min(getMinimumKey(leaf->left), getMinimumKey(leaf->right)));
}

// Public member
void BinaryTree::destroyTree()
{
    destroyTree(root);
}

int BinaryTree::createDuplicate()
{
    // Value returned that will be appended to the duplicate filename
    int index = 0;
    if (last_key_removed != 0) {
        insert(last_key_removed, root);
        index = last_key_removed;
        // Reset last_key_removed 
        last_key_removed = 0;
    }
    // Insert a new value equal to max_key + 1
    else {
        insert(getMaximumKey(root) + 1, root);
    }
    return index;
}

void BinaryTree::deleteNode(Node *leaf)
{

}

void BinaryTree::removeKey(int key)
{
    last_key_removed = key;    
    // Will return the node holding the key
    Node *leaf_to_remove = search(key, root);
    deleteNode(leaf_to_remove);
}



