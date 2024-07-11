#include "datawarehousestructs.hpp"

int main(int argc, char *argv[]) {
    using namespace std::chrono_literals;
    if (argc < 3)
    {
        std::cout << "Wrong number of arguments" << std::endl;
    }
    else 
    {
        std::string shm_name(argv[1]);
        int arr_capacity = atoi(argv[2]);

        SharedMemory shm;
        ServerSharedMemoryBuilder sharedMemoryBuilder(shm_name, arr_capacity);
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
        for (size_t i = 0; i < arr_capacity; i++)
        {
            std::string rec_name = "Record" + std::to_string(i+1);
            switch (i%5)
            {
            case 0:
                {
                    int32_t value = std::rand();
                    shm.addRecord(Record(i, rec_name, Record::int_type, value));
                }
                break;
            case 1:
                {
                    float value =  std::rand();
                    value /= 10;
                    shm.addRecord(Record(i, rec_name, Record::float_type, value));
                }
                break;
            case 2:
                {
                    double value = std::rand();
                    value /= 100;
                    shm.addRecord(Record(i, rec_name, Record::double_type, value));
                }
                break;
            case 3:
                {
                    uint64_t value = std::rand();
                    shm.addRecord(Record(i, rec_name, Record::uint_type, value));
                }
                break;
            case 4:
                {
                    std::string value = "VALUE";
                    shm.addRecord(Record(i, rec_name, Record::string_type, value));
                }          
                break;
            }
        }
        shm.getRecords();
    }
}