#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <random>
#include <map>
#include <set>
#include "Avl_tree.hpp"
#include "Set.hpp"

const size_t LOAD_SIZE = 1000000;

TEST(LoadTest, CompareAVLTreeWithStdMap){
    std::vector<int> keys(LOAD_SIZE);
    std::mt19937 gen(42);
    std::uniform_int_distribution<int> dist(0, 100000000);
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        keys[i] = dist(gen);
    }

    auto start_custom = std::chrono::high_resolution_clock::now();
    AVLTree<int, int> custom_tree;
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        custom_tree.insert(keys[i], static_cast<int>(i));
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        volatile bool found = custom_tree.key_be(keys[i]);
        (void)found;
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        custom_tree.remove(keys[i]);
    }
    auto end_custom = std::chrono::high_resolution_clock::now();
    double time_custom = std::chrono::duration<double, std::milli>(end_custom - start_custom).count();

    auto start_standard = std::chrono::high_resolution_clock::now();
    std::map<int, int> std_tree;
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        std_tree[keys[i]] = static_cast<int>(i);
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        volatile bool found = (std_tree.find(keys[i]) != std_tree.end());
        (void)found;
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        std_tree.erase(keys[i]);
    }
    auto end_standard = std::chrono::high_resolution_clock::now();
    double time_standard = std::chrono::duration<double, std::milli>(end_standard - start_standard).count();

    std::cout << "\n--- Сравнение деревьев (вставка, поиск и удаление " << LOAD_SIZE << " элементов) ---\n";
    std::cout << "Мой AVLTree:      " << time_custom << " ms\n";
    std::cout << "std::map:         " << time_standard << " ms\n";
}

TEST(LoadTest, CompareSetWithStdSet){
    std::vector<int> keys(LOAD_SIZE);
    std::mt19937 gen(123);
    std::uniform_int_distribution<int> dist(0, 100000000);
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        keys[i] = dist(gen);
    }

    auto start_custom = std::chrono::high_resolution_clock::now();
    Set<int> custom_set;
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        custom_set.insert(keys[i]);
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        volatile bool found = custom_set.contains(keys[i]);
        (void)found;
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        custom_set.remove(keys[i]);
    }
    auto end_custom = std::chrono::high_resolution_clock::now();
    double time_custom = std::chrono::duration<double, std::milli>(end_custom - start_custom).count();

    auto start_standard = std::chrono::high_resolution_clock::now();
    std::set<int> std_set;
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        std_set.insert(keys[i]);
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        volatile bool found = (std_set.find(keys[i]) != std_set.end());
        (void)found;
    }
    for (size_t i = 0; i < LOAD_SIZE; ++i){
        std_set.erase(keys[i]);
    }
    auto end_standard = std::chrono::high_resolution_clock::now();
    double time_standard = std::chrono::duration<double, std::milli>(end_standard - start_standard).count();

    std::cout << "\n--- Сравнение множеств (вставка, поиск и удаление " << LOAD_SIZE << " элементов) ---\n";
    std::cout << "Мой Set:          " << time_custom << " ms\n";
    std::cout << "std::set:         " << time_standard << " ms\n";
}

TEST(LoadTest, ExportBenchmarkForPlot){
    std::vector<size_t> sizes ={10000, 25000, 50000, 100000, 250000, 500000, 1000000};
    std::ofstream csv("benchmark.csv");
    csv << "N,AVL_Insert_ms,AVL_Search_ms,AVL_Remove_ms,StdMap_Insert_ms,StdMap_Search_ms,StdMap_Remove_ms\n";

    std::cout << "\n--- Замеры зависимости времени от N (запись в benchmark.csv) ---\n";
    for (size_t n : sizes){
        std::vector<int> keys(n);
        std::mt19937 gen(42);
        std::uniform_int_distribution<int> dist(0, 100000000);
        for (size_t i = 0; i < n; ++i){
            keys[i] = dist(gen);
        }

        AVLTree<int, int> avl;
        auto t1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) avl.insert(keys[i], static_cast<int>(i));
        auto t2 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i){
            volatile bool ok = avl.key_be(keys[i]);
            (void)ok;
        }
        auto t3 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) avl.remove(keys[i]);
        auto t4 = std::chrono::high_resolution_clock::now();

        std::map<int, int> mp;
        auto s1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) mp[keys[i]] = static_cast<int>(i);
        auto s2 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i){
            volatile bool ok = (mp.find(keys[i]) != mp.end());
            (void)ok;
        }
        auto s3 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < n; ++i) mp.erase(keys[i]);
        auto s4 = std::chrono::high_resolution_clock::now();

        double avl_ins = std::chrono::duration<double, std::milli>(t2 - t1).count();
        double avl_src = std::chrono::duration<double, std::milli>(t3 - t2).count();
        double avl_rem = std::chrono::duration<double, std::milli>(t4 - t3).count();

        double std_ins = std::chrono::duration<double, std::milli>(s2 - s1).count();
        double std_src = std::chrono::duration<double, std::milli>(s3 - s2).count();
        double std_rem = std::chrono::duration<double, std::milli>(s4 - s3).count();

        csv << n << "," << avl_ins << "," << avl_src << "," << avl_rem << "," << std_ins << "," << std_src << "," << std_rem << "\n";

        std::cout << "N = " << n << " | AVL(ins/src/rem): " << avl_ins << " / " << avl_src << " / " << avl_rem << " ms" << " | std::map: " << std_ins << " / " << std_src << " / " << std_rem << " ms\n";
    }
    csv.close();
}