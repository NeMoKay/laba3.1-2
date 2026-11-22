#pragma once
#include <iostream>
#include <stdexcept>
#include <stack>
#include <string>
#include <algorithm>


template <typename Key, typename Value = Key>
class AVLTree{
private:
    struct Node{
        Node(const Key& k, const Value& v);
        bool operator==(const Node& other) const;

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

    Node* copy_tree(Node* node) const;
    Node* delete_tree(Node* node);
    Node* locate(const Key& key) const;
    void insert_rec(Node*& ptr, const Key& key, const Value& val);
    void remove_rec(Node*& ptr, const Key& key);

    int len_rec(Node* node) const;

    void bypass_string(const std::string& order) const;
    template <typename Func> 
    void bypass_rec(Node* node, const std::string& order, Func action) const;

    bool IfInSubTree_rec(Node* node1, Node* node2) const;

    

public:
    AVLTree() = default;
    AVLTree(const AVLTree& other);
    AVLTree(AVLTree&& other) noexcept;

    AVLTree& operator=(const AVLTree& other);
    AVLTree& operator=(AVLTree&& other) noexcept;
    ~AVLTree();

    bool operator==(const AVLTree<Key, Value>& other) const;
    bool operator!=(const AVLTree<Key, Value>& other) const;



    void insert(const Key& key, const Value& val);
    void insert(const Key& key);

    void remove(const Key& key);
    Value search(const Key& key) const;
    bool key_be(const Key& key) const;

    int len() const;
    bool IsEmpty() const;
    Value Get(int index) const;

    template <typename Func> 
    void bypass(const std::string& order, Func action) const;


    AVLTree* TakeSubTree(const Key& key) const;
    bool IfInSubTree(const AVLTree<Key, Value>& sub) const;


    template <typename Func>
    AVLTree<Key, Value>* map(Func f) const;

    template <typename Predicate>
    AVLTree<Key, Value>* where(Predicate p) const;

    template <typename Func>
    Value reduce(Func f, const Value& start_value) const;



    class TreeIterator{
    private:
        std::stack<Node*> path;
        void push_left_branch(Node* node);
    public:
        TreeIterator(Node* root);
        bool HasNext() const;
        Value GetCurrentItem() const;
        Key GetCurrentKey() const;
        bool Next();

        Value operator*() const;
        TreeIterator& operator++();
        bool operator!=(const TreeIterator& other) const;
    };

    TreeIterator GetIterator() const;
    TreeIterator begin() const;
    TreeIterator end() const;

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
bool AVLTree<Key, Value>::Node::operator==(const Node& other) const {
    return key == other.key && val == other.val;
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
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::copy_tree(Node* node) const{

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
    else if(key < ptr->key){
        insert_rec(ptr->left, key, val);
    }
    else{
        ptr->val = val;
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
int AVLTree<Key, Value>::len_rec(Node* node) const{
    if(node == nullptr){
        return 0;
    }
    return 1 + len_rec(node->left) + len_rec(node->right);
}


template <typename Key, typename Value>
typename AVLTree<Key, Value>::Node* AVLTree<Key, Value>::locate(const Key& key) const{
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

template <typename Key, typename Value>
void AVLTree<Key, Value>::bypass_string(const std::string& order) const{
    if(order.length() != 3){
        throw std::invalid_argument("Несовподенние длинны команды обхода");
    }
    int c_K = 0;
    int c_L = 0;
    int c_P = 0;

    for(auto i : order){
        if(i == 'K'){
            c_K++;
        }
        if(i == 'L'){
            c_L++;
        }
        if(i == 'P'){
            c_P++;
        }
    }
    if(c_K != 1 || c_L != 1 || c_P != 1){
        throw std::invalid_argument("Несоответсвие команды обхода");
    }
}


template <typename Key, typename Value>
template <typename Func> 
void AVLTree<Key, Value>::bypass_rec(Node* node, const std::string& order, Func action) const{
    if(node == nullptr){
        return;
    }

    for(auto letter : order){
        if(letter == 'K'){
            action(node->key, node->val);
        }
        else if(letter == 'L'){
            bypass_rec(node->left, order, action);
        }
        else if (letter == 'P'){
            bypass_rec(node->right, order, action);
        }
    }

}


template <typename Key, typename Value>
bool AVLTree<Key, Value>::IfInSubTree_rec(Node* node1, Node* node2) const{
    if(node1 == nullptr || node2 == nullptr){
        return node1 == node2;
    }
    return (*node1 == *node2) && IfInSubTree_rec(node1->left, node2->left) && IfInSubTree_rec(node1->right, node2->right);
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
bool AVLTree<Key, Value>::operator==(const AVLTree<Key, Value>& other) const{
    return IfInSubTree_rec(root, other.root);
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::operator!=(const AVLTree<Key, Value>& other) const{
    return !(*this == other);
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
Value AVLTree<Key, Value>::search(const Key& key) const{
    Node* temp = locate(key);
    if(temp != nullptr){
        return temp->val;
    }
    else{
        throw std::invalid_argument("Key нет в древе"); 
    }
    
}



template <typename Key, typename Value>
bool AVLTree<Key, Value>::key_be(const Key& key) const{
    return locate(key) != nullptr;
}



template <typename Key, typename Value>
int AVLTree<Key, Value>::len() const{
    return len_rec(root);
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::IsEmpty() const{
    return root == nullptr;
}

template <typename Key, typename Value>
Value AVLTree<Key, Value>::Get(int index) const{
    if(index < 0){
        throw std::invalid_argument("Отрицательный индекс");
    }
    int i = 0;
    for(auto it = GetIterator(); it.HasNext(); it.Next()){
        if(i == index){
            return it.GetCurrentItem();
        }
        i++;
    }
    throw std::invalid_argument("Индекс выходит за пределы дерева");
}





template <typename Key, typename Value>
AVLTree<Key, Value>* AVLTree<Key, Value>::TakeSubTree(const Key& key) const{
    Node* temp = locate(key);
    if(temp == nullptr){
        return nullptr;
    }

    AVLTree<Key, Value>* new_tree = new AVLTree<Key, Value>();
    new_tree->root = copy_tree(temp);
    

    return new_tree;
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::IfInSubTree(const AVLTree<Key, Value>& sub) const{
    if(sub.root == nullptr){
        return true;
    }
    return IfInSubTree_rec(this->locate((sub.root)->key), sub.root);
}


template <typename Key, typename Value>
template <typename Func>
void AVLTree<Key, Value>::bypass(const std::string& order, Func action) const {
    bypass_string(order);
    bypass_rec(root, order, action);
}


//-------------


template <typename Key, typename Value>
template <typename Func>
AVLTree<Key, Value>* AVLTree<Key, Value>::map(Func f) const{
    AVLTree<Key, Value>* new_tree = new AVLTree<Key, Value>();
    for(auto it = GetIterator(); it.HasNext(); it.Next()){
        new_tree->insert(it.GetCurrentKey(), f(it.GetCurrentItem()));
    }
    return new_tree;
}

template <typename Key, typename Value>
template <typename Predicate>
AVLTree<Key, Value>* AVLTree<Key, Value>::where(Predicate f) const{
    AVLTree<Key, Value>* new_tree = new AVLTree<Key, Value>();

    for(auto it = GetIterator(); it.HasNext(); it.Next()){
        if(f(it.GetCurrentItem())){
            new_tree->insert(it.GetCurrentKey(), it.GetCurrentItem());
        }
    }
    return new_tree;
}



template <typename Key, typename Value>
template <typename Func>
Value AVLTree<Key, Value>::reduce(Func f, const Value& start_value) const{
    Value res = start_value;
    for(auto temp : *this){
       res = f(temp, res);
    }
    return res;
}

//--------------

template <typename Key, typename Value>
void AVLTree<Key, Value>::TreeIterator::push_left_branch(Node* node){

    Node* temp = node;
    while(temp != nullptr){
        path.push(temp);
        temp = temp->left;
    }
}

template <typename Key, typename Value>
AVLTree<Key, Value>::TreeIterator::TreeIterator(Node* root){
    push_left_branch(root);
}


template <typename Key, typename Value>
bool AVLTree<Key, Value>::TreeIterator::HasNext() const{
    return !path.empty();
}

template <typename Key, typename Value>
Value AVLTree<Key, Value>::TreeIterator::GetCurrentItem() const{
    if(path.empty()){
        throw std::invalid_argument("Список пуст");
    }
    return path.top()->val;
}

template <typename Key, typename Value>
Key AVLTree<Key, Value>::TreeIterator::GetCurrentKey() const{
    if(path.empty()){
        throw std::invalid_argument("Список пуст");
    }
    return path.top()->key;
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::TreeIterator::Next(){
    if(path.empty()){
        return false;
    }
    Node* curr = path.top();
    path.pop();
    push_left_branch(curr->right);
    return !path.empty();
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::TreeIterator AVLTree<Key, Value>::GetIterator() const{
    return TreeIterator(root);
}



template <typename Key, typename Value>
Value AVLTree<Key, Value>::TreeIterator::operator*() const{
    return GetCurrentItem();
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::TreeIterator& AVLTree<Key, Value>::TreeIterator::operator++(){
    Next();
    return *this;
}

template <typename Key, typename Value>
bool AVLTree<Key, Value>::TreeIterator::operator!=(const TreeIterator& other) const{
    if(path.empty() && other.path.empty()){
        return false;
    }
    if(path.empty() || other.path.empty()){
        return true;
    }
    return path.top() != other.path.top();
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::TreeIterator AVLTree<Key, Value>::begin() const{
    return TreeIterator(root);
}

template <typename Key, typename Value>
typename AVLTree<Key, Value>::TreeIterator AVLTree<Key, Value>::end() const{
    return TreeIterator(nullptr);
}