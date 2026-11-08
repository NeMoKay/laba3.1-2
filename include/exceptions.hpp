#pragma once

#include <string>
#include <exception>

class TreeException : public std::exception {
protected:
    std::string message;
public:
    explicit TreeException(const std::string& msg) : message(msg) {}
    explicit TreeException(const char* msg) : message(msg ? msg : "") {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

class KeyNotFoundException : public TreeException {
public:
    explicit KeyNotFoundException(const std::string& msg = "Key not found in tree") : TreeException(msg) {}
};

class DuplicateKeyException : public TreeException {
public:
    explicit DuplicateKeyException(const std::string& msg = "Key already exists in tree") : TreeException(msg) {}
};

class EmptyTreeException : public TreeException {
public:
    explicit EmptyTreeException(const std::string& msg = "Operation on empty tree") : TreeException(msg) {}
};

class InvalidArgumentException : public TreeException {
public:
    explicit InvalidArgumentException(const std::string& msg = "Invalid argument provided") : TreeException(msg) {}
};
