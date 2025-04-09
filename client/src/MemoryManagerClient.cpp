#include "MemoryManagerClient.h"

std::shared_ptr<grpc::Channel> MemoryManagerClient::channel_ = nullptr;

memory::CreateResponse MemoryManagerClient::Create(int size, const std::string& type) {
    memory::CreateRequest request;
    request.set_size(size);
    request.set_type(type);
    memory::CreateResponse response;
    grpc::ClientContext context;
    stub_->Create(&context, request, &response);
    return response;
}

memory::SetResponse MemoryManagerClient::Set(int id, const std::string& value) {
    memory::SetRequest request;
    request.set_id(id);
    request.set_value(value);
    memory::SetResponse response;
    grpc::ClientContext context;
    stub_->Set(&context, request, &response);
    return response;
}

memory::GetResponse MemoryManagerClient::Get(int id) {
    memory::GetRequest request;
    request.set_id(id);
    memory::GetResponse response;
    grpc::ClientContext context;
    stub_->Get(&context, request, &response);
    return response;
}

memory::RefCountResponse MemoryManagerClient::IncreaseRefCount(int id) {
    memory::RefCountRequest request;
    request.set_id(id);
    memory::RefCountResponse response;
    grpc::ClientContext context;
    stub_->IncreaseRefCount(&context, request, &response);
    return response;
}

memory::RefCountResponse MemoryManagerClient::DecreaseRefCount(int id) {
    memory::RefCountRequest request;
    request.set_id(id);
    memory::RefCountResponse response;
    grpc::ClientContext context;
    stub_->DecreaseRefCount(&context, request, &response);
    return response;
}
