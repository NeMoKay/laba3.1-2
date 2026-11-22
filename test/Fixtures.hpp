#pragma once

#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include <vector>
#include <cmath>
#include "Avl_tree.hpp"
#include "Set.hpp"


inline int double_val(int x){ return x * 2; }
inline int inc1(int x){ return x + 1; }
inline int inc2(int x){ return x + 2; }
inline int inc3(int x){ return x + 3; }

inline bool is_big(int x){ return x > 60; }
inline int sum_func(int x, int acc){ return x + acc; }
inline int lab_reduce_func(int x1, int x2){ return 2 * x1 + 3 * x2; }


struct Person{
    int id;
    std::string firstName;
    std::string lastName;

    bool operator<(const Person& other) const{ return id < other.id; }
    bool operator>(const Person& other) const{ return id > other.id; }
    bool operator==(const Person& other) const{
        return id == other.id && firstName == other.firstName && lastName == other.lastName;
    }
    bool operator!=(const Person& other) const{ return !(*this == other); }
};

inline std::ostream& operator<<(std::ostream& os, const Person& p){
    return os << "Person{id=" << p.id << ", " << p.firstName << " " << p.lastName << "}";
}

template <typename Key, typename Value>
inline std::string show_tree(const AVLTree<Key, Value>& tree){
    if (tree.IsEmpty()){
        return "[Пустое дерево]\n";
    }
    std::string res = "[len=" + std::to_string(tree.len()) + " | LKP: ";
    for (auto it = tree.GetIterator(); it.HasNext(); it.Next()){
        res += "(" + std::to_string(it.GetCurrentKey()) + ":" + std::to_string(it.GetCurrentItem()) + ") ";
    }
    res += "]\n";
    return res;
}

template <>
inline std::string show_tree(const AVLTree<std::string, std::string>& tree){
    if (tree.IsEmpty()){
        return "[Пустое дерево]\n";
    }
    std::string res = "[len=" + std::to_string(tree.len()) + " | LKP: ";
    for (auto it = tree.GetIterator(); it.HasNext(); it.Next()){
        res += "(" + it.GetCurrentKey() + ":" + it.GetCurrentItem() + ") ";
    }
    res += "]\n";
    return res;
}

template <typename T>
inline std::string show_set(const Set<T>& s){
    if (s.IsEmpty()){
        return "{Пустое множество}\n";
    }
    std::string res = "{len=" + std::to_string(s.len()) + " | элементы: ";
    for (int i = 0; i < s.len(); ++i){
        if (s.contains(i)){
            res += std::to_string(i) + " ";
        }
    }
    res += "}\n";
    return res;
}


class AVL_Fixture : public testing::Test{
protected:
    int keys_int[5] ={42, 52, 69, 67, 228};
    int vals_int[5] ={420, 520, 690, 670, 2280};

    AVLTree<int, int>* empty_tree;
    AVLTree<int, int>* tree_int;
    AVLTree<int, int>* orig_tree_int;
    AVLTree<double, double>* tree_double;
    AVLTree<std::string, std::string>* tree_str;

    void SetUp() override{
        empty_tree = new AVLTree<int, int>();
        tree_int = new AVLTree<int, int>();
        orig_tree_int = new AVLTree<int, int>();
        tree_double = new AVLTree<double, double>();
        tree_str = new AVLTree<std::string, std::string>();

        for (int i = 0; i < 5; ++i){
            tree_int->insert(keys_int[i], vals_int[i]);
            orig_tree_int->insert(keys_int[i], vals_int[i]);
        }

        tree_double->insert(1.5, 10.5);
        tree_double->insert(2.5, 20.5);
        tree_double->insert(0.5, 5.5);

        tree_str->insert("b", "второй");
        tree_str->insert("a", "первый");
        tree_str->insert("c", "третий");
    }

    void TearDown() override{
        delete empty_tree;
        delete tree_int;
        delete orig_tree_int;
        delete tree_double;
        delete tree_str;
    }
};

class Set_Fixture : public testing::Test{
protected:
    int data_A[5] ={42, 52, 69, 67, 228};
    int data_B[3] ={52, 69, 228};
    int data_C[3] ={1, 2, 3};

    Set<int>* empty_set;
    Set<int>* set_A;
    Set<int>* set_B;
    Set<int>* set_C;
    Set<std::string>* set_str;

    void SetUp() override{
        empty_set = new Set<int>();
        set_A = new Set<int>();
        set_B = new Set<int>();
        set_C = new Set<int>();
        set_str = new Set<std::string>();

        for (int x : data_A) set_A->insert(x);
        for (int x : data_B) set_B->insert(x);
        for (int x : data_C) set_C->insert(x);

        set_str->insert("красный");
        set_str->insert("зеленый");
        set_str->insert("синий");
    }

    void TearDown() override{
        delete empty_set;
        delete set_A;
        delete set_B;
        delete set_C;
        delete set_str;
    }
};