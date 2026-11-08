#include <iostream>
#include "avl_tree.hpp"

int main() {
    std::cout << "=== Лабораторная работа №1 (AVL Tree) ===" << std::endl;
    AVLTree<int> tree;
    std::cout << "Размер пустого дерева: " << tree.getSize() << std::endl;
    return 0;
}
