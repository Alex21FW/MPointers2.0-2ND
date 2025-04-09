#include "memory_manager_service.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <algorithm>
#include <vector>
#include <mutex>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>


MemoryManagerServiceImpl::MemoryManagerServiceImpl(size_t total_memory_size)
    : memory_pool_(new char[total_memory_size]),
      total_memory_(total_memory_size),
      used_memory_(0),
      current_id_(0),
      running_(true) {

    gc_thread_ = std::thread(&MemoryManagerServiceImpl::GarbageCollector, this);
}

MemoryManagerServiceImpl::~MemoryManagerServiceImpl() {
    running_ = false;
    if (gc_thread_.joinable())
        gc_thread_.join();
    delete[] memory_pool_;
}
void MemoryManagerServiceImpl::GarbageCollector() {
    while (running_) {
        std::this_thread::sleep_for(std::chrono::seconds(10));

        std::lock_guard<std::mutex> lock(memory_mutex);
        bool removed = false;

        for (auto it = memory_blocks_.begin(); it != memory_blocks_.end();) {
            if (it->second.ref_count == 0) {
                std::cout << "[GC] Eliminando bloque id: " << it->second.id << std::endl;
                it = memory_blocks_.erase(it);
                removed = true;
            } else {
                ++it;
            }
        }

        if (removed) {
            DumpMemoryState();
            Defragment();  // Ya contiene su propio DumpMemoryState()
        }
    }
}

void MemoryManagerServiceImpl::Defragment() {
    std::cout << "[GC] Desfragmentando memoria..." << std::endl;

    size_t offset = 0;
    std::vector<MemoryBlock> blocks;

    // Copia bloques a vector para ordenarlos por offset
    for (auto& pair : memory_blocks_)
        blocks.push_back(pair.second);

    std::sort(blocks.begin(), blocks.end(),
              [](const MemoryBlock& a, const MemoryBlock& b) {
                  return a.offset < b.offset;
              });

    for (auto& block : blocks) {
        if (block.offset != offset) {
            std::memmove(memory_pool_ + offset, memory_pool_ + block.offset, block.size);
            block.offset = offset;
        }
        offset += block.size;
        memory_blocks_[block.id] = block;
    }

    used_memory_ = offset;
    std::cout << "[GC] Memoria desfragmentada exitosamente." << std::endl;

    // 💾 Dump del estado de la memoria tras la desfragmentación
    DumpMemoryState();
}

// Método Create
grpc::Status MemoryManagerServiceImpl::Create(grpc::ServerContext* context,
    const memory::CreateRequest* request,
    memory::CreateResponse* response) {

    std::lock_guard<std::mutex> lock(memory_mutex);

    size_t size = request->size();
    const std::string& type = request->type();

    // 🚨 Ampliamos el tamaño si no es un tipo primitivo o string (para evitar Set fallido)
    if (!(type == "i" || type == "f" || type == "d" ||
          type == "NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE")) {
        size += 64;  // Añadir margen para objetos complejos
    }

    if (used_memory_ + size > total_memory_) {
        response->set_mensaje("Error: Memoria insuficiente.");
        response->set_id(-1);
        std::cerr << "[ERROR] Memoria insuficiente: solicitado (" << size << "), disponible (" 
                  << (total_memory_ - used_memory_) << ")" << std::endl;
        return grpc::Status::OK;
    }

    int new_id = ++current_id_;

    MemoryBlock new_block;
    new_block.id = new_id;
    new_block.size = size;
    new_block.type = type;
    new_block.data = "";
    new_block.ref_count = 1;
    new_block.offset = used_memory_;

    memory_blocks_[new_id] = new_block;
    used_memory_ += size;

    response->set_mensaje("Espacio creado correctamente.");
    response->set_id(new_id);

    std::cout << "[INFO] Espacio asignado (ID: " << new_id << "), tamaño: " << size << ", tipo: " << type << std::endl;

    DumpMemoryState();
    return grpc::Status::OK;
}


// Método Set
grpc::Status MemoryManagerServiceImpl::Set(grpc::ServerContext* context, 
    const memory::SetRequest* request,
    memory::SetResponse* response) {

    std::lock_guard<std::mutex> lock(memory_mutex);

    int id = request->id();
    const std::string& value = request->value();

    auto it = memory_blocks_.find(id);
    if (it == memory_blocks_.end()) {
        response->set_mensaje("Error: ID inválido.");
        std::cerr << "[ERROR] Set falló: ID inválido (" << id << ")" << std::endl;
        return grpc::Status::OK;
    }

    if (value.size() > it->second.size) {
        response->set_mensaje("Error: tamaño de valor excede espacio reservado.");
        std::cerr << "[ERROR] Set falló: tamaño de valor excede (" << value.size() << ") > (" << it->second.size << ")" << std::endl;
        return grpc::Status::OK;
    }

    // Guardar el dato en el bloque
    it->second.data = value;

    size_t offset = it->second.offset; 
    memcpy(memory_pool_ + offset, value.data(), value.size());

    response->set_mensaje("Valor establecido correctamente.");
    std::cout << "[INFO] Valor establecido en ID: " << id << ", valor: " << value << std::endl;

    // 🟢 DUMP después de modificar la memoria
    DumpMemoryState();

    return grpc::Status::OK;
}


// Método Get
grpc::Status MemoryManagerServiceImpl::Get(grpc::ServerContext* context,
    const memory::GetRequest* request, memory::GetResponse* response) {

    std::lock_guard<std::mutex> lock(memory_mutex);

    int id = request->id();

    auto it = memory_blocks_.find(id);
    if (it == memory_blocks_.end()) {
        response->set_mensaje("Error: ID inválido.");
        response->set_value("");
        response->set_type("");
        std::cerr << "[ERROR] Get falló: ID inválido (" << id << ")" << std::endl;
        return grpc::Status::OK;
    }

    response->set_mensaje("Valor obtenido correctamente.");
    response->set_value(it->second.data);
    response->set_type(it->second.type);

    std::cout << "[INFO] Valor obtenido en ID: " << id
              << ", valor: " << it->second.data
              << ", tipo: " << it->second.type << std::endl;

    return grpc::Status::OK;
}

// Método IncreaseRefCount
grpc::Status MemoryManagerServiceImpl::IncreaseRefCount(grpc::ServerContext* context,
    const memory::RefCountRequest* request, memory::RefCountResponse* response) {

    std::lock_guard<std::mutex> lock(memory_mutex);

    auto it = memory_blocks_.find(request->id());
    if (it != memory_blocks_.end()) {
        it->second.ref_count++;
        response->set_ref_count(it->second.ref_count);
        response->set_success(true);
        response->set_mensaje("Referencia incrementada correctamente.");

        // 🟢 Dump luego de incrementar ref count
        DumpMemoryState();

    } else {
        response->set_success(false);
        response->set_mensaje("Id no encontrado.");
    }

    return grpc::Status::OK;
}


// Método DecreaseRefCount
grpc::Status MemoryManagerServiceImpl::DecreaseRefCount(grpc::ServerContext* context,
    const memory::RefCountRequest* request, memory::RefCountResponse* response) {

    std::lock_guard<std::mutex> lock(memory_mutex);

    auto it = memory_blocks_.find(request->id());
    if (it != memory_blocks_.end()) {
        if (it->second.ref_count > 0) {
            it->second.ref_count--;
        }

        response->set_ref_count(it->second.ref_count);
        response->set_success(true);

        if (it->second.ref_count == 0) {
            used_memory_ -= it->second.size;
            memory_blocks_.erase(it);
            response->set_mensaje("Referencia decrementada y bloque eliminado.");
        } else {
            response->set_mensaje("Referencia decrementada correctamente.");
        }

        // 🟢 Dump luego de modificación del ref count (ya sea que se borre o no)
        DumpMemoryState();

    } else {
        response->set_success(false);
        response->set_mensaje("Id no encontrado.");
    }

    return grpc::Status::OK;
}


void MemoryManagerServiceImpl::DumpMemoryState() {
    const std::string folder = "./dumps";  // Modificación aquí
    if (!std::filesystem::exists(folder)) {
        std::filesystem::create_directories(folder); // Usa create_directories en vez de create_directory
    }

    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();

    std::ostringstream filename;
    filename << folder << "/dump_" << ms << ".txt";

    std::ofstream file(filename.str());
    if (!file.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo de dump: " << filename.str() << std::endl;
        return;
    }

    file << "===== DUMP DE MEMORIA =====\n";
    file << "Timestamp (ms): " << ms << "\n";
    file << "Memoria total: " << total_memory_ << "\n";
    file << "Memoria usada: " << used_memory_ << "\n";
    file << "Bloques:\n";

    for (const auto& [id, block] : memory_blocks_) {
        file << "ID: " << id 
             << ", Tamaño: " << block.size 
             << ", Offset: " << block.offset
             << ", Tipo: " << block.type 
             << ", Valor: " << block.data 
             << ", RefCount: " << block.ref_count << "\n";
    }

    file.close();
    std::cout << "[INFO] Dump de memoria creado: " << filename.str() << std::endl;
}
