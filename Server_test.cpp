#include "datawarehousestructs.hpp"
#include <thread>

int main(){
    std::string name = "Server record";
    char value[] = {'A','B','O','B','A'};
    ServerSharedMemoryBuilder sharedMemoryBuilder = ServerSharedMemoryBuilder(name, 5);
    SharedMemoryDirector director;
    director.buildSharedMemory(sharedMemoryBuilder);
    SharedMemory shm = sharedMemoryBuilder.getResult();
    Record first = Record(123, name, Record::int_type, 124);
    Record second = Record(125, name, Record::double_type, 124.15);
    Record third = Record(124, name, Record::char_ptr_type, *value);
    shm.addRecord(first);
    shm.getRecords();
    shm.addRecord(second);
    shm.getRecords();
    shm.addRecord(third);
    shm.getRecords();
    shm.getStats();
    shm.getRecordById(123);
}