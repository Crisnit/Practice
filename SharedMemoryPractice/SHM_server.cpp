#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

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

    ftruncate(shm_fd, SIZE);

    double *ptr = (double*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (size_t i = 0; i < SIZE; i+=sizeof(double))
    {
        ptr[i] = 1.12312312;
    }

    munmap(ptr, SIZE);

    close(shm_fd);

    return EXIT_SUCCESS;
}