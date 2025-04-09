cmake_minimum_required(VERSION 3.10)
project(mpointer_client)

set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/../memory_manager/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(COMMON_SOURCES
    src/MPointer.cpp
    ${CMAKE_SOURCE_DIR}/../memory_manager/src/memory_manager.pb.cc
    ${CMAKE_SOURCE_DIR}/../memory_manager/src/memory_manager.grpc.pb.cc
)

# Cliente original con opciones interactivas
add_executable(mpointer_client
    src/main.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(mpointer_client
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)

# Cliente demo actual (que ya tienes)
add_executable(demo_client
    src/demo.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(demo_client
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)

# NUEVO cliente para demo de LinkedList
add_executable(linkedlist_demo
    src/linkedlist_demo.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(linkedlist_demo
    gRPC::grpc++
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)


