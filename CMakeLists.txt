cmake_minimum_required(VERSION 3.22)

project(shared_memory_practice)

set(SOURCE_SERVER_EXE SHM_server.cpp)

set(SOURCE_CLIENT_EXE SHM_client.cpp)

add_compile_options(-fsanitize=address)

add_compile_options(-fsanitize=undefined)

add_link_options(-fsanitize=address)

add_link_options(-fsanitize=undefined)

include_directories(shared_memory_structs)

add_executable(shm_server ${SOURCE_SERVER_EXE})

add_executable(shm_client ${SOURCE_CLIENT_EXE})

add_subdirectory(shared_memory_structs)

target_link_libraries(shm_server PRIVATE shared_memory_structs)

link_libraries(shm_client PRIVATE shared_memory_structs)