#include "MPointer.h"
#include "LinkedList.h"
#include <iostream>

int main() {
    MPointerBase::Init("localhost:50051");

    LinkedList<int> lista;
    lista.insert(10);
    lista.insert(20);
    lista.insert(30);

    std::cout << "Lista creada: ";
    lista.print();

    return 0;
}
