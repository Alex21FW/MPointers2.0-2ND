#include <iostream>
#include "MPointer.tpp"
#include "MPointer.h"

int main(int argc, char** argv) {
    // Inicialización del cliente
    MPointerBase::Init("localhost:50051");

    std::cout << "🚩 [DEMO] Inicializando demostración del estado de memoria.\n";

    // Crear punteros remotos
    MPointer<int> entero = MPointer<int>::New();
    MPointer<std::string> texto = MPointer<std::string>::New();
    MPointer<float> flotante = MPointer<float>::New();

    std::cout << "[DEMO] Punteros remotos creados exitosamente.\n";

    // Asignar valores
    entero = 42;
    texto = std::string("Hola Mundo");
    flotante = 3.1415f;

    std::cout << "[DEMO] Valores asignados a punteros remotos.\n";
    std::cout << "  Entero: " << *entero << "\n";
    std::cout << "  Texto: " << *texto << "\n";
    std::cout << "  Flotante: " << *flotante << "\n";

    // Copiar referencias (Incremento de referencias)
    MPointer<int> entero2 = entero;
    std::cout << "[DEMO] Copia realizada, referencias incrementadas.\n";

    // Liberar referencias (Decremento de referencias)
    entero2.~MPointer();  // Forzamos destrucción para ver decremento inmediato
    std::cout << "[DEMO] Referencia liberada (entero2 destruido).\n";

    std::cout << "[DEMO] Fin de la demostración, revisa los dumps en servidor.\n";

    return 0;
}
