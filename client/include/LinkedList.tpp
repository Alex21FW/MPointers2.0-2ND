#include "LinkedList.h"
#include <iostream>

template<typename T>
void LinkedList<T>::insert(const T& value) {
    MPointer<Nodo<T>> nuevoNodo = MPointer<Nodo<T>>::New();

    Nodo<T> temp;
    temp.valor = value;
    temp.siguiente = MPointer<Nodo<T>>(); // nullptr por defecto

    nuevoNodo.Set(temp);  // Usamos Set en lugar de ->

    if (size == 0) {
        head = nuevoNodo;
    } else {
        MPointer<Nodo<T>> actual = head;
        while (actual.Get().siguiente.GetID() != 0) {
            actual = actual.Get().siguiente;
        }

        Nodo<T> ultimo = actual.Get();
        ultimo.siguiente = nuevoNodo;
        actual.Set(ultimo);
    }

    size++;
}

template<typename T>
void LinkedList<T>::print() const {
    MPointer<Nodo<T>> actual = head;

    std::cout << "[";
    while (actual.GetID() != 0) {
        Nodo<T> temp = actual.Get();
        std::cout << temp.valor;

        actual = temp.siguiente;
        if (actual.GetID() != 0)
            std::cout << " -> ";
    }
    std::cout << "]\n";
}

