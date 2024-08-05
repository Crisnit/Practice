#include "datawarehousestructs.hpp"
#include <map>

const std::map<std::string,Record::type> valueTypes = 
{
    {"int", Record::type::T_INT_TYPE},
    {"double", Record::type::T_DOUBLE_TYPE},
    {"float", Record::type::T_FLOAT_TYPE},
    {"unsigned_int", Record::type::T_UINT_TYPE},
    {"string", Record::type::T_STRING_TYPE}
};

int main(int argc, char *argv[]) {
    if (argc < 2)
    {
        std::cout << "Wrong number of arguments" << std::endl;
        return 0;
    }

    if (argc == 2 && std::string(argv[1]) == "--help")
    {
        std::cout << "Available commands: " << std::endl;
        std::cout << "shm_client <memory_name> --show" << std::endl;
        std::cout << "shm_client <memory_name> --show <id>" << std::endl;
        std::cout << "shm_client <memory_name> --set <id> <value_type> <value>" << std::endl;
        return 0;
    }
    
    std::string shm_name(argv[1]);
    
    ClientSharedMemoryBuilder sharedMemoryBuilder(shm_name);
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

    std::string command(argv[2]);
    if (command == "--show")
    {
        if (argc < 4)
        {
            shm.getRecords();
            shm.getStats();
        }
        else 
        {
            int id = atoi(argv[3]);
            shm.getRecordById(id);
        }
    }
    else if (command == "--set" && argc == 6)
    {
        int id = atoi(argv[3]);
        std::string value_type(argv[4]);
        if (auto v_type = valueTypes.find(value_type); v_type == valueTypes.end())
        {
            std::cout << "Unknown type" << std::endl;
            return 0;
        }
        else
        {
            switch (v_type->second)
            {
            case Record::type::T_FLOAT_TYPE:
                {
                    float value = atof(argv[5]);
                    shm.changeRecordValue(id,v_type->second, value);
                }
                break;
            case Record::type::T_DOUBLE_TYPE:
                {
                    double value = std::stod(argv[5]);
                    shm.changeRecordValue(id,v_type->second, value);
                }
            case Record::type::T_INT_TYPE:
                {
                    int32_t value = std::stoi(argv[5]);
                    shm.changeRecordValue(id,v_type->second, value);
                }
            case Record::type::T_UINT_TYPE:
                {
                    uint64_t value = std::stoull(argv[5]);
                    shm.changeRecordValue(id,v_type->second, value);
                }
            case Record::type::T_STRING_TYPE:
                {
                    std::string value(argv[5]);
                    shm.changeRecordValue(id,v_type->second, value);
                }
            }
        }   
    }
}
