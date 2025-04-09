#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"  // Asegúrate de que la ruta de los headers sea correcta

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class MemoryManagerClient {
    public:
        static void Init(const std::string& server_address) {
            channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
        }
    
        MemoryManagerClient()
            : stub_(memory::MemoryManager::NewStub(channel_)) {}
    
        void interactive() {
            int choice;
            do {
                std::cout << "Seleccione acción:\n"
                          << "1. Create\n"
                          << "2. Set\n"
                          << "3. Get\n"
                          << "4. IncreaseRefCount\n"
                          << "5. DecreaseRefCount\n"
                          << "6. Salir\n"
                          << "Opción: ";
    
                std::cin >> choice;
    
                switch (choice) {
                    case 1: {
                        int size;
                        std::string type;
                        std::cout << "Tamaño: ";
                        std::cin >> size;
                        std::cout << "Tipo: ";
                        std::cin >> type;
                        auto response = Create(size, type);
                        std::cout << "Id creado: " << response.id() << " mensaje: " << response.mensaje() << "\n";
                        break;
                    }
                    case 2: {
                        int id;
                        std::string value;
                        std::cout << "Id: ";
                        std::cin >> id;
                        std::cout << "Valor: ";
                        std::cin >> value;
                        auto response = Set(id, value);
                        std::cout << response.mensaje() << "\n";
                        break;
                    }
                    case 3: {
                        int id;
                        std::cout << "Id: ";
                        std::cin >> id;
                        auto get_response = Get(id);
                        std::cout << "Valor obtenido: " << get_response.value() 
                                  << " | Tipo: " << get_response.type()
                                  << " | Mensaje: " << get_response.mensaje() << "\n";
                        break;
                    }
                    case 4: {
                        int id;
                        std::cout << "Id: ";
                        std::cin >> id;
                        auto response = IncreaseRefCount(id);
                        std::cout << "Referencias incrementadas: " << response.ref_count() 
                                  << " mensaje: " << response.mensaje() << "\n";
                        break;
                    }
                    case 5: {
                        int id;
                        std::cout << "Id: ";
                        std::cin >> id;
                        auto response = DecreaseRefCount(id);
                        std::cout << "Referencias decrementadas: " << response.ref_count() 
                                  << " mensaje: " << response.mensaje() << "\n";
                        break;
                    }
                    case 6:
                        std::cout << "Saliendo...\n";
                        break;
                    default:
                        std::cout << "Opción no válida, intente de nuevo.\n";
                        break;
                }
    
            } while (choice != 6);
        }
    
        memory::CreateResponse Create(int size, const std::string& type) {
            memory::CreateRequest request;
            request.set_size(size);
            request.set_type(type);
    
            memory::CreateResponse response;
            ClientContext context;
            stub_->Create(&context, request, &response);
    
            return response;
        }
    
        memory::SetResponse Set(int id, const std::string& value) {
            memory::SetRequest request;
            request.set_id(id);
            request.set_value(value);
    
            memory::SetResponse response;
            ClientContext context;
            stub_->Set(&context, request, &response);
    
            return response;
        }
    
        memory::GetResponse Get(int id) {
            memory::GetRequest request;
            request.set_id(id);
    
            memory::GetResponse response;
            ClientContext context;
            stub_->Get(&context, request, &response);
    
            return response;
        }
    
        memory::RefCountResponse IncreaseRefCount(int id) {
            memory::RefCountRequest request;
            request.set_id(id);
    
            memory::RefCountResponse response;
            ClientContext context;
            stub_->IncreaseRefCount(&context, request, &response);
    
            return response;
        }
    
        memory::RefCountResponse DecreaseRefCount(int id) {
            memory::RefCountRequest request;
            request.set_id(id);
    
            memory::RefCountResponse response;
            ClientContext context;
            stub_->DecreaseRefCount(&context, request, &response);
    
            return response;
        }
    
    private:
        std::unique_ptr<memory::MemoryManager::Stub> stub_;
        static std::shared_ptr<grpc::Channel> channel_;
    };
    
    std::shared_ptr<grpc::Channel> MemoryManagerClient::channel_ = nullptr;
    
    int main(int argc, char** argv) {
        MemoryManagerClient::Init("localhost:50051");
        MemoryManagerClient client;
        client.interactive();
    
        return 0;
    }