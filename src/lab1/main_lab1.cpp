#include "avl_tree.hpp"



int plus(int val){
    return val+1;
}

bool is_chet(int val){
    return val%2 == 0;
}

int sum(int val, int back_sum){
    return val + back_sum;
}

int main(){
    AVLTree<int, int> temp;
    temp.insert(1);
    temp.insert(2);
    temp.insert(3);
    temp.insert(4);
    temp.insert(5);
    temp.insert(6);

    AVLTree<int, int>* ptr1 = temp.map(plus);
    AVLTree<int, int>* ptr2 = temp.where(is_chet);
    
    int bob = temp.reduce(sum, 0);
    int bob2 = ptr2->reduce(sum, 0);


    


    std::cout << ptr1->search(1) << "\n";
    std::cout << ptr2->search(2) << " " << ptr2->search(4) <<  "\n";
    std::cout << bob << "\n";
    std::cout << bob2 << "\n";







    return 0;
}
