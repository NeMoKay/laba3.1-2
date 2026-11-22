#pragma once
#include "avl_tree.hpp"

template <typename T>
class Set{
private:
    AVLTree<T,T> tree;
public:
    Set() = default;
    Set(const Set<T>& other) = default;
    Set(Set<T>&& other) noexcept = default;
    Set<T>& operator=(const Set<T>& other) = default;
    Set<T>& operator=(Set<T>&& other) noexcept = default;
    ~Set() = default;

    void insert(const T& item);
    void remove(const T& item);
    bool contains(const T& item) const;

    int len() const;
    bool IsEmpty() const;

    bool If_Set_in(const Set<T>& other) const;
    bool operator==(const Set<T>& other) const;
    bool operator!=(const Set<T>& other) const;
    
    


};

template <typename T>
void Set<T>::insert(const T& item){
    tree.insert(item);
}

template <typename T>
void Set<T>::remove(const T& item){
    tree.remove(item);
}

template <typename T>
bool Set<T>::contains(const T& item) const{
    return tree.key_be(item);
}

template <typename T>
int Set<T>::len()const{
    return tree.len();
}

template <typename T>
bool Set<T>::IsEmpty() const{
    return tree.IsEmpty();
}

template <typename T>
bool Set<T>::If_Set_in(const Set<T>& other) const{
    for(int i = 0; i < this->len(); i++){
        if(!other.contains(tree.Get(i))){
            return 0;
        }
    }
    return 1;
}

template <typename T>
bool Set<T>::operator==(const Set<T>& other) const{
    return other.If_Set_in(*this) && this->If_Set_in(other);
}

template <typename T>
bool Set<T>::operator!=(const Set<T>& other) const{
    return !(*this == other);
}


