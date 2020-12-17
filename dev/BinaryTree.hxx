#include "Node.hxx"

#include <iostream>

class BinaryTree {
    public:
        ~BinaryTree();
        void preOrder();
        void insert(int key);
        void deleteNode(int key);
        int createDuplicate();

    private:
        Node *insert(Node *node, int key);
        Node *newNode(Node *n, int key);
        Node *rightRotate(Node *y);
        Node *leftRotate(Node *y);
        Node *deleteNode(Node *n, int key);

        Node *getMinimumValueNode(Node *n);
        Node *getMaximumValueNode(Node *n);

        int getBalance(Node *n);
        int getMinimumKey(Node *n);
        int getMaximumKey(Node *n);

        void preOrder(Node *r);
        int height(Node *n);

        void destroyTree(Node *n);

        Node *root = nullptr;
        /* Keep track of the last value removed, so we know that it should be the next one to be added 
         * if != 0. */
        int last_key_removed = 0;
};
