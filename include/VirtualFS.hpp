#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "Avl_tree.hpp"


class VirtualFS{
public:
    struct FSNode{
        std::string name;
        std::string path;
        bool is_directory = false;
        std::string real_path;

        FSNode() = default;
        FSNode(const std::string& name, const std::string& path, bool is_directory, const std::string& real_path = "");
    };

private:
    AVLTree<std::string, FSNode> FS_tree;
    std::string now_path = "/";

    std::string make_full_path(const std::string& path) const;
    void create_node(const std::string& path, bool is_directory, const std::string& real_path = "");

public:
    VirtualFS();

    void mkdir(const std::string& path);

    void touch(const std::string& path);
    void touch(const std::string& path, const std::string& real_path);

    void rm(const std::string& path);
    void mv(const std::string& old_path, const std::string& new_path);
    std::vector<FSNode> ls(const std::string& path = "") const;
    std::string cat(const std::string& path) const;
    void cd(const std::string& path);
    std::string pwd() const;
};


inline VirtualFS::FSNode::FSNode(const std::string& name, const std::string& path, bool is_directory, const std::string& real_path)
    : name(name), path(path), is_directory(is_directory), real_path(real_path) {}



inline std::string VirtualFS::make_full_path(const std::string& path) const{
    if(path.empty() || path == "."){
        return now_path;
    }

    if(path == ".."){
        if(now_path == "/"){
            return "/";
        }
        size_t pos = now_path.find_last_of('/');
        if(pos == 0){
            return "/";
        } 
        else{
            return now_path.substr(0, pos);
        }
    }

    if(path[0] == '/'){
        return path;
    }

    if(now_path == "/"){
        return now_path + path;
    } 
    else{
        return now_path + "/" + path;
    }
}

inline void VirtualFS::create_node(const std::string& path, bool is_directory, const std::string& real_path){
    std::string full_path = make_full_path(path);

    if(FS_tree.key_be(full_path)){
        throw std::invalid_argument("Уже добавлен " + full_path);
    }

    size_t pos = full_path.find_last_of('/');
    std::string parent_path;
    if(pos == 0){
        parent_path = "/";
    }
    else{
        parent_path = full_path.substr(0, pos);
    }

    if(!FS_tree.key_be(parent_path)){
        throw std::invalid_argument("Родительская директория не найдена: " + parent_path);
    }

    FSNode parent = FS_tree.search(parent_path);
    if(!parent.is_directory){
        throw std::invalid_argument("Родитель не является директорией: " + parent_path);
    }

    std::string name = full_path.substr(pos + 1);
    FS_tree.insert(full_path, FSNode(name, full_path, is_directory, real_path));
}

inline VirtualFS::VirtualFS(){
    FS_tree.insert("/", FSNode("/", "/", true));
}

inline void VirtualFS::mkdir(const std::string& path){
    create_node(path, true);
}

inline void VirtualFS::touch(const std::string& path){
    std::string full_path = make_full_path(path);
    std::string disk_path = full_path.substr(1);
    touch(full_path, disk_path);
}


inline void VirtualFS::touch(const std::string& path, const std::string& real_path){
    std::ifstream file(real_path);
    if(!file.is_open()){
        throw std::invalid_argument("Реальный файл не найден: " + real_path);
    }

    create_node(path, false, real_path);
}


inline void VirtualFS::rm(const std::string& path){
    std::string full_path = make_full_path(path);

    if(full_path == "/"){
        throw std::invalid_argument("Нельзя удалить корневую директорию");
    }

    if(!FS_tree.key_be(full_path)){
        throw std::invalid_argument("Узел не найден: " + full_path);
    }

    FSNode node = FS_tree.search(full_path);
    if(node.is_directory && !ls(full_path).empty()){
        throw std::invalid_argument("Директория не пуста: " + full_path);
    }

    FS_tree.remove(full_path);
}


inline void VirtualFS::mv(const std::string& old_path, const std::string& new_path){
    std::string full_old = make_full_path(old_path);
    std::string full_new = make_full_path(new_path);

    if(!FS_tree.key_be(full_old)){
        throw std::invalid_argument("Узел не найден: " + full_old);
    }

    FSNode node = FS_tree.search(full_old);
    if(node.is_directory){
        throw std::invalid_argument("Перемещение директорий не поддерживается: " + full_old);
    }

    create_node(full_new, false, node.real_path);
    FS_tree.remove(full_old);
}


inline std::vector<VirtualFS::FSNode> VirtualFS::ls(const std::string& path) const{
    std::string full_path = make_full_path(path);

    if(!FS_tree.key_be(full_path)){
        throw std::invalid_argument("Узел не найден");
    }
    FSNode file = FS_tree.search(full_path);
    if(!file.is_directory){
        return {file};
    }

    std::string prefix;
    if(full_path == "/"){
        prefix = "/";
    } 
    else{
        prefix = full_path + "/";
    }

    std::vector<FSNode> res;
        for(const auto& node : FS_tree){
            if(node.path == full_path){
                continue;
            }
            if(node.path.rfind(prefix, 0) == 0 && node.path.find('/', prefix.size()) == std::string::npos){
                res.push_back(node);
            } 
            else if(!res.empty() && node.path.rfind(prefix, 0) != 0){
                break;
            }
        }

    return res;
}

inline std::string VirtualFS::cat(const std::string& path) const {
    std::string full_path = make_full_path(path);

    if(!FS_tree.key_be(full_path)){
        throw std::invalid_argument("Файл не найден: " + full_path);
    }

    FSNode file = FS_tree.search(full_path);
    if(file.is_directory){
        throw std::invalid_argument("Это директория, а не файл: " + full_path);
    }

    std::ifstream in(file.real_path);
    if(!in.is_open()){
        throw std::runtime_error("Не удалось открыть реальный файл: " + file.real_path);
    }

    std::string text;
    std::string line;
    while(std::getline(in, line)){
        text += line + "\n";
    }
    return text;
}

inline void VirtualFS::cd(const std::string& path) {
    std::string full_path = make_full_path(path);

    if(!FS_tree.key_be(full_path)){
        throw std::invalid_argument("Директория не найдена: " + full_path);
    }

    FSNode node = FS_tree.search(full_path);
    if(!node.is_directory){
        throw std::invalid_argument("Невозможно перейти в файл: " + full_path);
    }

    now_path = full_path;
}

inline std::string VirtualFS::pwd() const{
    return now_path;
}