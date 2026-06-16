#pragma once

#include "exceptions.h"

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(const T& val, Node* n = nullptr) : data(val), next(n) {}
    };

    Node* topNode;

public:
    Stack() : topNode(nullptr) {}

    ~Stack() {
        clear();
    }

    void push(const T& val) {
        topNode = new Node(val, topNode);
    }

    void pop() {
        if (isEmpty()) {
            throw MathException("Стек пуст, невозможно удалить элемент.");
        }
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
    }

    T top() const {
        if (isEmpty()) {
            throw MathException("Стек пуст.");
        }
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }
};
