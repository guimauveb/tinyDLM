#include <iostream>
#include <map>

#include "../helper/include/BinaryTree.hxx"


int main(int argc, char **argv)
{
    // Store duplicate filenames into a binary tree
    BinaryTree *bt = new BinaryTree;

    bt->insert(2);
    bt->insert(1);
    bt->insert(5);
    bt->insert(4);

    std::cout << "maximum key = " << bt->getMaximumKey() << '\n';
    std::cout << "minimum key = " << bt->getMinimumKey() << '\n';

    delete bt;

    return 0;
}
