#pragma once

#include <gtest/gtest.h>
#include "avl_tree.hpp"
#include "exceptions.hpp"

class AVLTreeFixture : public testing::Test {
protected:
    AVLTree<int> empty_tree;
    AVLTree<int> sample_tree;

    void SetUp() override {
        // Подготовка тестовых данных
    }

    void TearDown() override {
        // Очистка
    }
};

class VFSCommandFixture : public testing::Test {
protected:
    void SetUp() override {
        // Инициализация файловой системы
    }

    void TearDown() override {
        // Очистка
    }
};
