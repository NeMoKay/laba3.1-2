#pragma once
#include <iostream>
#include <algorithm>



template <typename Key, typename Value = Key>
class AVLTree{
private:
    struct Node{
        Node(const Key& k, const Value& v);

        Key key;
        Value val;

        Node* left;
        Node* right;

        int height;
    };
    Node* root = nullptr;

    int getHeight(Node* node);
    int set_new_hight(Node* node);
    int balance_factor(Node* node);

    void left_spin(Node*& node);
    void right_spin(Node*& node);
    void balance(Node*& node);

    Node* copy_tree(Node* node);
    Node* delete_tree(Node* node);
    Node* locate(const Key& key);
    void insert_rec(Node*& ptr, const Key& key, const Value& val);
    void remove_rec(Node*& ptr, const Key& key);



public:
    AVLTree() = default;
    AVLTree(const AVLTree& other);
    AVLTree(AVLTree&& other) noexcept;

    AVLTree& operator=(const AVLTree& other);
    AVLTree& operator=(AVLTree&& other) noexcept;
    ~AVLTree();


    void insert(const Key& key, const Value& val);
    void insert(const Key& key);

    void remove(const Key& key);
    Value search(const Key& key);
    bool key_be(const Key& key);

    AVLTree* TakeSubTree(const Key& key);


    // AVLTree<Key, Value>* map(Func f);
    // AVLTree<Key, Value>* where(Predicate p);
    // Value reduce(Func f, const Value& start_value);

};



template <typename Key, typename Value>
AVLTree<Key, Value>::Node::Node(const Key& k, const Value& v){
    key = k;
    val = v;

    left = nullptr;
    right = nullptr;

    height = 0;
}
template <typename Key, typename Value>
int AVLTree<Key, Value>::getHeight(Node* node){
    if(node == nullptr){
        return -1;
    }
    else{
        return node->height;
    }
}

template <typename Key, typename Value>
int AVLTree<Key, Value>::set_new_hight(Node* node){
    if(node == nullptr){
        throw std::invalid_argument("node = nullptr");
    }
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    return node->height;
}

template <typename Key, typename Value>
int AVLTree<Key, Value>::balance_factor(Node* node){
    if(node == nullptr){
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::left_spin(Node*& node){
    if(node == nullptr || node->right == nullptr){
        return;
    }

    Node* left_children = (node->right)->left;
    Node* temp = node;
    node = node->right;
    node->left = temp;
    temp->right = left_children;

    set_new_hight(temp);
    set_new_hight(node);
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::right_spin(Node*& node){
    if(node == nullptr || node->left == nullptr){
        return;
    }
    Node* right_children = (node->left)->right;
    Node* temp = node;
    node = node->left;
    node->right = temp;
    temp->left = right_children;

    set_new_hight(temp);
    set_new_hight(node);
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::balance(Node*& node){
    
    if (node == nullptr){
        return;
    }

    set_new_hight(node);
    int bf = balance_factor(node);
    if(bf == 0 || bf == 1 || bf == -1){
        return;
    }
    else{
        if(bf == 2){
            if(balance_factor(node->left) == -1){
                left_spin(node->left);
                right_spin(node);
            }
            else{
                right_spin(node);
            }
        }

        else{
            if(balance_factor(node->right) == 1){
                right_spin(node->right);
                left_spin(node);
            }
            else{
                left_spin(node);
            }
        }
    }
}



template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::copy_tree(Node* node){

    if(node == nullptr){
        return nullptr;
    }
    Node* new_node = new Node(node->key, node->val);
    new_node->height = node->height;
    new_node->right = copy_tree(node->right);
    new_node->left = copy_tree(node->left);

    return new_node;
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::delete_tree(Node* node){
    if(node == nullptr){
        return nullptr;
    }
    else{
        node->left = delete_tree(node->left);
        node->right = delete_tree(node->right);
    }

    delete node;
    return nullptr;
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::insert_rec(Node*& ptr, const Key& key, const Value& val){
    if(ptr == nullptr){
        ptr = new Node(key, val);
        return;
    }
    if(key > ptr->key){
        insert_rec(ptr->right, key, val);
    }

    if(key < ptr->key){
        insert_rec(ptr->left, key, val);

    }

    if(key == ptr->key){
        return;
    }

    balance(ptr);
    
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::remove_rec(Node*& ptr, const Key& key){
    if(ptr == nullptr){
        return;
    }
    if(key > ptr->key){
        remove_rec(ptr->right, key);
    }
    else if(key < ptr->key){
        remove_rec(ptr->left, key);

    }
    else{
        if(ptr->left == nullptr){
            Node* temp = ptr;
            ptr = ptr->right;
            delete temp;
        }
        else if(ptr->right == nullptr){
            Node* temp = ptr;
            ptr = ptr->left;
            delete temp;
        }
        else{
            Node* temp = ptr->right;
            while(temp->left != nullptr){
                temp = temp->left;
            }
            ptr->key = temp->key;
            ptr->val = temp->val;
            remove_rec(ptr->right, ptr->key);
        }

    }

    if(ptr != nullptr){
        balance(ptr);
    }
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::locate(const Key& key){
    Node* temp = root;
    while(temp != nullptr){
        if(key > temp->key){
            temp = temp->right;
            continue;
        }
        if(key < temp->key){
            temp = temp->left;
            continue;
        }
        if(key == temp->key){
            return temp;
        }
    }
    return nullptr;
}

//------------


template <typename Key, typename Value>
AVLTree<Key, Value>::AVLTree(const AVLTree& other){
    root = copy_tree(other.root);
}

template <typename Key, typename Value>
AVLTree<Key, Value>::AVLTree(AVLTree&& other) noexcept{
    root = other.root;
    other.root = nullptr;
}

template <typename Key, typename Value>
AVLTree<Key, Value>::~AVLTree(){
    delete_tree(root);
}


template <typename Key, typename Value>
AVLTree<Key, Value>& AVLTree<Key, Value>::operator=(const AVLTree& other){
    if(&other == this){
        return *this;
    }
    delete_tree(root);
    root = copy_tree(other.root);
    return *this;
}

template <typename Key, typename Value>
AVLTree<Key, Value>& AVLTree<Key, Value>::operator=(AVLTree&& other) noexcept{
    if(&other == this){
        return *this;
    }
    delete_tree(root);
    root = other.root;
    other.root = nullptr;
    return *this;
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::insert(const Key& key, const Value& val){
    insert_rec(root, key, val);
}

template <typename Key, typename Value>
void AVLTree<Key, Value>::insert(const Key& key){
    insert_rec(root, key, key);
}


template <typename Key, typename Value>
void AVLTree<Key, Value>::remove(const Key& key){
    remove_rec(root, key);
}

template <typename Key, typename Value>
Value AVLTree<Key, Value>::search(const Key& key){
    Node* temp = locate(key);
    if(temp != nullptr){
        return temp->val;
    }
    else{
        throw std::invalid_argument("Key нет в древе"); 
    }
    
}


   


template <typename Key, typename Value>
bool AVLTree<Key, Value>::key_be(const Key& key){
    return locate(key) != nullptr;
}


template <typename Key, typename Value>
AVLTree<Key, Value>* AVLTree<Key, Value>::TakeSubTree(const Key& key){
    Node* temp = locate(key);
    if(temp == nullptr){
        return nullptr;
    }

    AVLTree<Key, Value>* new_tree = new AVLTree<Key, Value>();
    new_tree->root = copy_tree(temp);
    

    return new_tree;
}