#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include<iostream>

const int NUM = 3;
const int SIZE = NUM * sizeof(int);
const char* NAME = "SHM";

int main(){
    int shm_fd;


    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open()");
        return EXIT_FAILURE;
    }

    int *ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (size_t i = 0; i < NUM; i++)
    {
        std::cout << ptr[i];
    }

    std::cout << "Hello, World!";

    shm_unlink(NAME);
    return 0;
}
