#ifndef MEMORY_MANAGER_SERVICE_H
#define MEMORY_MANAGER_SERVICE_H

#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"
#include <unordered_map>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>

struct MemoryBlock {
    int id;
    size_t offset;   
    int size;
    std::string type;
    std::string data;
    int ref_count;
};

class MemoryManagerServiceImpl final : public memory::MemoryManager::Service {
public:
    explicit MemoryManagerServiceImpl(size_t total_memory_size);
    ~MemoryManagerServiceImpl();

    grpc::Status Create(grpc::ServerContext*, const memory::CreateRequest*, memory::CreateResponse*) override;
    grpc::Status Set(grpc::ServerContext*, const memory::SetRequest*, memory::SetResponse*) override;
    grpc::Status Get(grpc::ServerContext*, const memory::GetRequest*, memory::GetResponse*) override;
    grpc::Status IncreaseRefCount(grpc::ServerContext*, const memory::RefCountRequest*, memory::RefCountResponse*) override;
    grpc::Status DecreaseRefCount(grpc::ServerContext*, const memory::RefCountRequest*, memory::RefCountResponse*) override;

private:
    char* memory_pool_;               // puntero inicial a la memoria reservada
    size_t total_memory_;             // tamaño total de memoria disponible
    size_t used_memory_;              // memoria usada actualmente
    int current_id_;                  // id actual disponible

    std::mutex memory_mutex;          // protección para concurrencia

    // Uso consistente de la estructura MemoryBlock
    std::unordered_map<int, MemoryBlock> memory_blocks_; // mapa id -> MemoryBlock
    std::thread gc_thread_;
    std::atomic<bool> running_;

    void GarbageCollector();
    void Defragment();
    void DumpMemoryState();
    std::string dump_folder_ = "./dumps";
};

#endif // MEMORY_MANAGER_SERVICE_H