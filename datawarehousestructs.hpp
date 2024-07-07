#include <cstdint>
#include <cstring>
#include <chrono>
#include <sys/mman.h>

union ValueType{
    float fl;
    double db;
    int32_t i;
    uint64_t ui;
    char ch[32];
};

struct Record{
public:
    enum Type {float_type, double_type, int_type, uint_type, char_type};
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
        case char_type:
            std::memcpy(m_value.ch, input_value, 32);
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

struct SharedMemory{
public:
    void setName(char &name){
        std::memcpy(m_name, &name, 16);
    }

    void setArrSize(int array_size){
        m_arr_size = array_size;
    }

    void setMemSize(int memory_size){
        m_memory_size = memory_size;
    }
    
    ~SharedMemory(){
        shm_unlink(m_name);
    }

private:
    char m_name[16];
    int m_arr_size;
    int m_arr_count;
    int m_memory_size;
    int m_elem_size = sizeof(Record);
};

struct SharedMemoryServer : SharedMemory{
public:
    SharedMemoryServer(char& name, int arr_size): m_arr_size(arr_size), m_arr_count(0){
        std::memcpy(m_name, &name, 16);
        m_memory_size = m_elem_size * m_arr_size;
    }
private:
    char m_name[16];
    int m_arr_size;
    int m_arr_count;
    int m_memory_size;
    int m_elem_size = sizeof(Record);
};

/*class SharedMemoryBuilder{
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