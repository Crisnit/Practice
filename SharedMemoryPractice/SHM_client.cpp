#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

const int NUM = 3;
const int SIZE = NUM * sizeof(double);
const char* NAME = "SHM";

int main(){
    int shm_fd;


    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open()");
        return EXIT_FAILURE;
    }

    double *ptr = (double*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (size_t i = 0; i < SIZE; i+=sizeof(double))
    {
        std::cout << ptr[i]<<std::endl;
    }
    struct stat buf;
    fstat(shm_fd, &buf);
    std::cout << buf.st_size;

    shm_unlink(NAME);

    return 0;
}
