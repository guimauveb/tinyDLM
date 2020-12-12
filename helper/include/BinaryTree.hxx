#include "Node.hxx"

class BinaryTree {
    public:
        BinaryTree();
        ~BinaryTree();

        void insert(int key);
        Node *search(int key);
        void destroy_tree();

    private:
        void destroy_tree(Node *leaf);
        void insert(int key, Node *leaf);
        Node *search(int key, Node *leaf);

        Node *root;
};
