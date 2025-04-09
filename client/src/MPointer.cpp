#include "MPointer.h"
#include <iostream>  // Para std::cout

// Definición del miembro estático
std::unique_ptr<memory::MemoryManager::Stub> MPointerBase::stub_ = nullptr;

// Implementación del método Init fuera de MPointer.h
void MPointerBase::Init(const std::string& server_address) {
    auto channel = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    stub_ = memory::MemoryManager::NewStub(channel);

    if (stub_) {
        std::cout << "[INFO] MPointer conectado a servidor en " << server_address << std::endl;
    } else {
        std::cerr << "[ERROR] No se pudo establecer la conexión con el servidor." << std::endl;
    }
}

