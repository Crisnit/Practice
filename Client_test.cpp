#include "datawarehousestructs.hpp"
#include <thread>

int main(int argc, char *argv[]) {
    std::string name = "MEMORY";
    char value[] = {'A','B','O','B','A'};
    ClientSharedMemoryBuilder sharedMemoryBuilder(name);
    SharedMemoryDirector director;
    director.buildSharedMemory(sharedMemoryBuilder);
    SharedMemory shm;
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
    shm.getRecords();
    shm.getRecords();
    shm.getRecords();
    shm.changeRecordValue(123, Record::Type::double_type, 1);
    shm.getRecords();
    shm.getStats();
    //shm.deleteSharedMemory();
}