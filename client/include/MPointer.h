#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "memory_manager.grpc.pb.h"

class MPointerBase {
protected:
    static std::unique_ptr<memory::MemoryManager::Stub> stub_;

public:
    static void Init(const std::string& server_address);
    static memory::MemoryManager::Stub* Stub() {
        return stub_.get();
    }
};

template<typename T>
class MPointer : public MPointerBase {
public:
    MPointer();
    MPointer(std::nullptr_t);
    MPointer(const T& value);
    ~MPointer();

    static MPointer<T> New();

    void Set(const T& value);
    T Get() const;

    MPointer(const MPointer& other);
    MPointer& operator=(const MPointer& other);
    MPointer& operator=(const T& value);

    T operator*() const;
    T* operator->();

    int GetID() const;

private:
    int id_;
    mutable std::unique_ptr<T> cached_value_;
};

#include "MPointer.tpp"  // Aquí sí se incluye la implementación de templates

#endif // MPOINTER_H
