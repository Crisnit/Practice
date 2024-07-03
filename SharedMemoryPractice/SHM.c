#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>



int main(){
    const int NUM = 3;
    const int SIZE = NUM * sizeof(int);
    const char* NAME = "SHM";


    int shm_fd;


    shm_fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open()");
        return EXIT_FAILURE;
    }

    ftruncate(shm_fd, SIZE);

    int *ptr = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (size_t i = 0; i < NUM; i++)
    {
        ptr[i] = i;
    }

    munmap(ptr, SIZE);

    close(shm_fd);

    return EXIT_SUCCESS;
}