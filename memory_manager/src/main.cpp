#include <iostream>
#include <string>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"
#include <memory>
#include "memory_manager_service.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

 
int main(int argc, char** argv) {
    std::string server_address("0.0.0.0:50051");
    size_t total_memory_size = 1024 * 1024; // Por ejemplo, 1 MB
    
    MemoryManagerServiceImpl service(total_memory_size);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Servidor ejecutándose en: " << server_address << std::endl;

    server->Wait();

    return 0;
}
    