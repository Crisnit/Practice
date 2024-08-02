cmake_minimum_required(VERSION 3.22)

project(shared_memory_practice)

set(SOURCE_SERVER_EXE SHM_server.cpp)

set(SOURCE_CLIENT_EXE SHM_client.cpp)

set(SOURCE_LIB datawarehousestructs.hpp)

add_library(datawarehousestructs INTERFACE)

target_include_directories(datawarehousestructs INTERFACE)

add_executable(shm_server ${SOURCE_SERVER_EXE})

add_executable(shm_client ${SOURCE_CLIENT_EXE})

target_link_libraries(shm_server PRIVATE datawarehousestructs)

target_link_libraries(shm_client PRIVATE datawarehousestructs)