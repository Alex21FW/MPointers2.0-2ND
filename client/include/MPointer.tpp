#ifndef MPOINTER_TPP
#define MPOINTER_TPP

#include <iostream>
#include <type_traits>
#include "MPointer.h"
#include <sstream>

// Constructor por defecto
template<typename T>
MPointer<T>::MPointer() {
    memory::CreateRequest request;
    request.set_size(sizeof(T));
    request.set_type(typeid(T).name());

    memory::CreateResponse response;
    grpc::ClientContext context;

    auto status = stub_->Create(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("Create failed: " + status.error_message());
    }

    id_ = response.id();
}

// Constructor con valor
template<typename T>
MPointer<T>::MPointer(const T& value) : MPointer() {
    Set(value);
}

// Destructor
template<typename T>
MPointer<T>::~MPointer() {
    memory::RefCountRequest request;
    memory::RefCountResponse response;
    grpc::ClientContext context;
    request.set_id(id_);
    stub_->DecreaseRefCount(&context, request, &response);
}

// Método New estático
template<typename T>
MPointer<T> MPointer<T>::New() {
    return MPointer<T>();
}

// Método Set
template<typename T>
void MPointer<T>::Set(const T& value) {
    memory::SetRequest request;
    request.set_id(id_);

    if constexpr (std::is_same<T, std::string>::value) {
        request.set_value(value);
    } else if constexpr (std::is_arithmetic<T>::value) {
        request.set_value(std::to_string(value));
    } else {
        // Para estructuras/clases personalizadas, se guarda un marcador simbólico
        std::ostringstream oss;
        oss << "custom_object@" << reinterpret_cast<const void*>(&value);
        request.set_value(oss.str());  // Esto evita romper el protocolo
    }

    memory::SetResponse response;
    grpc::ClientContext context;

    auto status = stub_->Set(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("Set failed: " + status.error_message());
    }
}

// Método Get
template<typename T>
T MPointer<T>::Get() const {
    memory::GetRequest request;
    request.set_id(id_);
    memory::GetResponse response;
    grpc::ClientContext context;

    auto status = stub_->Get(&context, request, &response);
    if (!status.ok()) {
        throw std::runtime_error("Get failed: " + status.error_message());
    }

    if constexpr (std::is_same<T, int>::value) {
        return std::stoi(response.value());
    } else if constexpr (std::is_same<T, float>::value) {
        return std::stof(response.value());
    } else if constexpr (std::is_same<T, double>::value) {
        return std::stod(response.value());
    } else if constexpr (std::is_same<T, std::string>::value) {
        return response.value();
    } else {
        // Para tipos personalizados, devolvemos objeto por defecto (constructor por defecto)
        return T{};
    }
}


// Constructor copia
template<typename T>
MPointer<T>::MPointer(const MPointer& other) {
    id_ = other.id_;
    memory::RefCountRequest request;
    request.set_id(id_);
    memory::RefCountResponse response;
    grpc::ClientContext context;
    stub_->IncreaseRefCount(&context, request, &response);
}

// operador=
template<typename T>
MPointer<T>& MPointer<T>::operator=(const MPointer& other) {
    if (this != &other) {
        memory::RefCountRequest request;
        request.set_id(id_);
        memory::RefCountResponse response;
        grpc::ClientContext context;
        stub_->DecreaseRefCount(&context, request, &response);

        id_ = other.id_;

        grpc::ClientContext ctx2;
        request.set_id(id_);
        stub_->IncreaseRefCount(&ctx2, request, &response);
    }
    return *this;
}

// operador= con valor
template<typename T>
MPointer<T>& MPointer<T>::operator=(const T& value) {
    Set(value);
    return *this;
}

// operador*
template<typename T>
T MPointer<T>::operator*() const {
    return Get();
}

// operador->
template<typename T>
T* MPointer<T>::operator->() {
    static_assert(std::is_class<T>::value, "operator-> solo disponible para tipos clase/struct");
    if (!cached_value_) {
        cached_value_ = std::make_unique<T>(Get());
    }
    return cached_value_.get();
}

// GetID
template<typename T>
int MPointer<T>::GetID() const {
    return id_;
}

template<typename T>
MPointer<T>::MPointer(std::nullptr_t) : id_(0), cached_value_(nullptr) {
    // constructor vacío que inicializa id en cero (sin puntero remoto asignado aún)
}


#endif // MPOINTER_TPP
