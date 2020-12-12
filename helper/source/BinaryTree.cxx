#include "../include/BinaryTree.hxx"

BinaryTree::BinaryTree()
{
    root = nullptr;
}

BinaryTree::~BinaryTree()
{
    destroy_tree();
}

void BinaryTree::destroy_tree(node *leaf)
{

}
