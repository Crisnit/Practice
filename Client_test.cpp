#include "datawarehousestructs.hpp"
#include <thread>

int main(){
    std::string name = "Client record";
    char value[] = {'A','B','O','B','A'};
    ClientSharedMemoryBuilder sharedMemoryBuilder = ClientSharedMemoryBuilder(name);
    SharedMemoryDirector director;
    director.buildSharedMemory(sharedMemoryBuilder);
    SharedMemory shm = sharedMemoryBuilder.getResult();
    Record first = Record(123, name, Record::int_type, 124);
    Record second = Record(125, name, Record::double_type, 124.15);
    Record third = Record(124, name, Record::char_ptr_type, *value);
    shm.getRecords();
    shm.getRecords();
    shm.getRecords();
    shm.getRecords();
    shm.getStats();
}