#ifndef NODO_H
#define NODO_H

#include "MPointer.h"

template<typename T>
struct Nodo {
    T valor;
    MPointer<Nodo<T>> siguiente;

    Nodo() : siguiente(nullptr) {}
};

#endif
