#include <iostream>
#include <map>

#include <vector>

#include "../helper/include/BinaryTree.hxx"

void testDuplicateRoutine(int key, std::map<std::string, BinaryTree*>& filenames_records, const std::string& filename)
{
    // new url added:
    // check if there is an entry for the filename
    auto it = filenames_records.find(filename);
    if (it != filenames_records.end()) {
        std::cout << filename << " already present. Creating a duplicate.\n";
        //          (createDuplicate())
        //          Check if the pointer is not nullptr -> is not, create a binary tree and insert 1
        if (filenames_records[filename] == nullptr) {
            //   std::cout << "Creating a binary tree for " << filename << '\n';
            filenames_records[filename] = new BinaryTree;
            //  std::cout << "Duplicate index is " << filenames_records[filename]->getMaximumKey() << '\n';
        }
        //          else check what was the last_value_removed if last_value_removed is != 0
        else {
            // std::cout << "Last key removed = " << last_key_rm << "\n"; 
            // if last_value_removed != 0 
            // insert last_value_removed, set it to 0 and duplicate is == last_value_removed
            // else insert max_key and duplicate == max_key + 1
            std::cout << "inserting key " << key << '\n';
            filenames_records[filename]->insert(key);
            //filenames_records[filename]->createDuplicate();
            //   std::cout << "New key == getMaxKey + 1\n" << filenames_records[filename]->getMaximumKey() << '\n';
        }
    }
    else {
        // if no, create an entry with a nullptr
        std::cout << "Create an nullptr entry for " << filename << '\n';
        filenames_records[filename] = nullptr;
    }

}

int main(int argc, char **argv)
{
    // Store duplicate filenames into a binary tree
    std::map<std::string, BinaryTree*> f_records;

    std::vector<const std::string> str_vec = {"filename", "filename", "filename", "filename", "filename", "filename"};

    for (int i = 1; i < (int)str_vec.size(); ++i) {
        testDuplicateRoutine(i, f_records, str_vec[i]);
    }
    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';

    f_records["filename"]->deleteNode(2);
    f_records["filename"]->deleteNode(3);
    f_records["filename"]->deleteNode(5);

    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';
    std::cout << f_records["filename"]->createDuplicate() << '\n';




    return 0;
}

