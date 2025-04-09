#ifndef MEMORY_MANAGER_CLIENT_H
#define MEMORY_MANAGER_CLIENT_H

#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"

class MemoryManagerClient {
public:
    static MemoryManagerClient& Instance() {
        static MemoryManagerClient instance(channel_);
        return instance;
    }

    static void Init(const std::string& server_address) {
        channel_ = grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
    }

    memory::CreateResponse Create(int size, const std::string& type);
    memory::SetResponse Set(int id, const std::string& value);
    memory::GetResponse Get(int id);
    memory::RefCountResponse IncreaseRefCount(int id);
    memory::RefCountResponse DecreaseRefCount(int id);

private:
    explicit MemoryManagerClient(std::shared_ptr<grpc::Channel> channel)
        : stub_(memory::MemoryManager::NewStub(channel)) {}

    MemoryManagerClient(const MemoryManagerClient&) = delete;
    MemoryManagerClient& operator=(const MemoryManagerClient&) = delete;

    static std::shared_ptr<grpc::Channel> channel_;
    std::unique_ptr<memory::MemoryManager::Stub> stub_;
};

#endif // MEMORY_MANAGER_CLIENT_H
