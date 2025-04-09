#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "Nodo.h"
#include "MPointer.h"

template<typename T>
class LinkedList {
private:
    MPointer<Nodo<T>> head;
    int size;

public:
    LinkedList() : head(nullptr), size(0) {}

    void insert(const T& value);
    void print() const;
};

#include "LinkedList.tpp"

#endif
