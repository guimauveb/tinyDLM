#include "Node.hxx"

class BinaryTree {
    public:
        BinaryTree();
        ~BinaryTree();

        void insert(int key);
        Node *search(int key);
        void destroyTree();

        // DOING
        int createDuplicate();
        void removeKey(int key);

    private:
        void deleteNode(Node *leaf);
        void destroyTree(Node *leaf);
        void insert(int key, Node *leaf);
        Node *search(int key, Node *leaf);
        int getMinimumKey(Node *leaf);
        int getMaximumKey(Node *leaf);

        Node *root;
        // Keep track of the last value removed, so we know that it should be the next one to be added 
        // if != 0.
        int last_key_removed = 0;
};
