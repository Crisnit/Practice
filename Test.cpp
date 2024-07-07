#include "datawarehousestructs.hpp"

int main(){
    char name[] = {'A','B','O','B','A'};
    SharedMemory shm = SharedMemory(*name, 5);
    Record first = Record(123, name, Record::int_type, 124);
    Record second = Record(125, name, Record::double_type, 124.15);
    Record third = Record(124, name, Record::char_ptr_type, *name);
    shm.addRecord(first);
    shm.getRecords();
    shm.addRecord(second);
    shm.getRecords();
    shm.addRecord(third);
    shm.getRecords();
}