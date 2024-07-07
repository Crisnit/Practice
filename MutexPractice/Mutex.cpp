#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std::chrono_literals;
const int NUM = 3;
const int SIZE = NUM * sizeof(double);
const char* NAME = "SHM";
std::mutex mutex;

void SHM_server(){
    mutex.lock();
    int shm_fd;


    shm_fd = shm_open(NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shm_fd < 0) {
        shm_unlink(NAME);
        shm_fd = shm_open(NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    }
    
    ftruncate(shm_fd, SIZE);

    double *ptr = (double*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    std::this_thread::sleep_for(10000ms);
    for (size_t i = 0; i < SIZE; i+=sizeof(double))
    {
        ptr[i] = 1.12312312;
    }

    munmap(ptr, SIZE);

    close(shm_fd);
    mutex.unlock();
}

void SHM_client(){
    mutex.lock();
    int shm_fd;


    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);

    double *ptr = (double*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (size_t i = 0; i < SIZE; i+=sizeof(double))
    {
        std::cout << ptr[i]<<std::endl;
    }
    struct stat buf;
    fstat(shm_fd, &buf);
    std::cout << buf.st_size;

    shm_unlink(NAME);
    mutex.unlock();
}
int main(){
    std::thread thread1(SHM_server);
    std::thread thread2(SHM_client);
    
    

    thread1.join();
    thread2.join();
    return 0;
}