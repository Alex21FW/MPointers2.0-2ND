cmake_minimum_required(VERSION 3.10)
project(memory_manager)

set(CMAKE_CXX_STANDARD 17)

# Busca las librerías instaladas de Protobuf y gRPC
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Incluye los directorios necesarios
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)

# Agrega los archivos fuente
set(SERVER_SOURCES
    src/main.cpp
    src/memory_manager_service.cpp
    src/memory_manager.pb.cc
    src/memory_manager.grpc.pb.cc
)

add_executable(memory_manager ${SERVER_SOURCES})
target_link_libraries(memory_manager
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)
