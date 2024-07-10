#include "datawarehousestructs.hpp"
#include <thread>

int main(){
    std::string name = "MEMORY";
    char value[] = {'A','B','O','B','A'};
    SharedMemory shm;
    ServerSharedMemoryBuilder sharedMemoryBuilder(name, 5);
    SharedMemoryDirector director;
    director.buildSharedMemory(sharedMemoryBuilder);
    try
    {
        shm = sharedMemoryBuilder.getResult();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 0;
    }
    Record first = Record(123, name, Record::int_type, 124);
    Record second = Record(125, name, Record::double_type, 124.15);
    Record third = Record(124, name, Record::char_type, *value);
    shm.addRecord(first);
    shm.getRecords();
    shm.addRecord(second);
    shm.getRecords();
    shm.addRecord(third);
    shm.getRecords();
    shm.getStats();
    shm.getRecordById(123);
}