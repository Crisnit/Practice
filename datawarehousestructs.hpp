#include <cstdint>
#include <cstring>
#include <chrono>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

union ValueType{
    float fl;
    double db;
    int32_t i;
    uint64_t ui;
    char ch[32];
};

struct Record{
public:
    enum Type {float_type, double_type, int_type, uint_type, char_ptr_type};
    template <typename T> Record(uint32_t id, char name[64], Type type, T input_value): m_id(id), m_type(type)
    {
        std::memcpy(m_name, name, 64);
        setValue(input_value);
        m_timestamp = getTimestamp();
    };


private:
    template <typename T>
    void setValue(T input_value){
        switch (m_type)
        {
        case float_type:
            m_value.fl = input_value;
            break;
        case double_type:
            m_value.db = input_value;
            break;
        case int_type:
            m_value.i = input_value;
            break;
        case uint_type:
            m_value.ui = input_value;
            break;
        case char_ptr_type:
            std::memcpy(m_value.ch, &input_value, 32);
            break;
        }
    }

    uint64_t getTimestamp(){
        std::chrono::_V2::system_clock::duration time = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(time).count();
    }

    uint32_t m_id;
    char m_name[64];
    uint64_t m_timestamp;
    Type m_type;
    ValueType m_value;
};

class SharedMemory{
public:
    SharedMemory(char& name, int arr_size): m_arr_size(arr_size), m_arr_count(0){
        std::memcpy(m_name, &name, 16);

        m_shm_fd = shm_open(m_name, O_CREAT | O_EXCL | O_RDWR, 0666);
        if (m_shm_fd < 0) {
            throw std::runtime_error("Shared memory with this name already exists");
        }

        m_memory_size = m_arr_size * m_elem_size;

        ftruncate(m_shm_fd, m_memory_size);

        m_arr_ptr = (Record*)mmap(0, m_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_shm_fd, 0);
    }
    
    void addRecord(Record record){
        if (m_arr_size > m_arr_count)
        {
            std::mutex lock;
            m_arr_ptr[m_arr_count] = record;
            m_arr_count++;
            std::mutex unlock;
        }
        
    }

    void getRecords(){
        if (m_arr_count > 0){
            std::mutex lock;
            for (size_t i = 0; i < m_arr_count; i++)
            {
                std::cout << &m_arr_ptr[i] << std::endl;
            }
            std::mutex unlock;
        }
    }

    ~SharedMemory(){
        shm_unlink(m_name);
    }

private:
    int m_shm_fd;
    Record* m_arr_ptr;
    char m_name[16];
    int m_arr_size;
    int m_arr_count;
    int m_memory_size;
    int m_elem_size = sizeof(Record);
};

/*class SharedMemoryClient{
public:
    SharedMemoryClient(char& name){
        std::memcpy(m_name, &name, 16);

        m_shm_fd = shm_open(m_name, O_CREAT | O_RDWR, 0666);

        struct stat buf;
        fstat(m_shm_fd, &buf);
        m_memory_size = buf.st_size;

        m_arr_size = m_memory_size / m_elem_size;

        m_arr_ptr = (Record*) mmap(0, m_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_shm_fd, 0);
    }
    
    ~SharedMemoryClient(){
        munmap(m_arr_ptr, m_memory_size);
        close(m_shm_fd);
    }

private:
    int m_shm_fd;
    Record* m_arr_ptr;
    char m_name[16];
    int m_arr_size;
    int m_memory_size;
    int m_elem_size = sizeof(Record);
};

class SharedMemoryBuilder{
public:
    virtual void buildName(char &name) = 0;
    virtual void buildArrSize(int array_size) = 0;
    virtual void buildMemSize(int array_size) = 0;
    virtual SharedMemory getResult() = 0;
};
class ServerSharedMemoryBuilder : public SharedMemoryBuilder{
public:
    ServerSharedMemoryBuilder(){
        m_shared_memory = SharedMemory();
    }
    void buildName(char &name) override{
        m_shared_memory.setName(name);
    }
    void buildArrSize(int array_size) override{
        m_shared_memory.setArrSize(array_size);
    }
    void buildMemSize(int array_size) override{
        int memory_size = array_size * RECORD_SIZE;
        m_shared_memory.setMemSize(memory_size);
    }
    SharedMemory getResult() override{
        return m_shared_memory;
    }
    private:
    SharedMemory m_shared_memory;
};
class ClientSharedMemoryBuilder : public SharedMemoryBuilder{
public:
    ClientSharedMemoryBuilder(){
        m_shared_memory = SharedMemory();
    }
    void buildName(char &name) override{
        m_shared_memory.setName(name);
    }
    void buildArrSize(int array_size) override{
        m_shared_memory.setArrSize(array_size);
    }
    void buildMemSize(int array_size) override{
        int memory_size = array_size * RECORD_SIZE;
        m_shared_memory.setMemSize(memory_size);
    }
    SharedMemory getResult() override{
        return m_shared_memory;
    }
    private:
    SharedMemory m_shared_memory;
};
class SHMApi{
public:
    SHMApi(){
        m_shm = SharedMemory();
    }
    void buildServer(char &name, int arr_size);
private:
    Record* m_ptr;
    SharedMemory m_shm;
};*/