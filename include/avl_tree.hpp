#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <algorithm>
#include "exceptions.hpp"

template <typename Key, typename Value = Key>
class AVLTree {
public:
    struct Node {
        Key key;
        Value value;
        int height;
        Node* left;
        Node* right;

        Node(const Key& k, const Value& v)
            : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
            
        Node(const Key& k)
            : key(k), value(k), height(1), left(nullptr), right(nullptr) {}
    };

private:
    Node* root;
    size_t size_;

    // Вспомогательные методы работы с высотой и балансом
    int getHeight(Node* node) const {
        return node ? node->height : 0;
    }

    void updateHeight(Node* node) {
        if (node) {
            node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
        }
    }

    int getBalanceFactor(Node* node) const {
        return node ? (getHeight(node->left) - getHeight(node->right)) : 0;
    }

public:
    AVLTree() : root(nullptr), size_(0) {}
    
    ~AVLTree() {
        clear();
    }

    size_t getSize() const {
        return size_;
    }

    bool isEmpty() const {
        return root == nullptr;
    }

    void clear() {
        // Очистка дерева (пост-ордер)
        std::function<void(Node*)> freeNode = [&](Node* node) {
            if (!node) return;
            freeNode(node->left);
            freeNode(node->right);
            delete node;
        };
        freeNode(root);
        root = nullptr;
        size_ = 0;
    }
};
