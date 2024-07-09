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

class SharedMemoryMutex {
public:
    void init(pthread_mutex_t &t_lock) {
        pthread_mutexattr_t attr;

        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&t_lock, &attr);
    };
    void lock(pthread_mutex_t &t_lock) {
        pthread_mutex_lock(&t_lock);
    };
    void unlock(pthread_mutex_t &t_lock) {
        pthread_mutex_unlock(&t_lock);
    };
};

union ValueType {
    float fl;
    double db;
    int32_t i;
    uint64_t ui;
    char ch[32];
};

struct Record{
public:
    enum Type {float_type, double_type, int_type, uint_type, char_ptr_type};
    template <typename T> Record(uint32_t id, std::string &t_name, Type type, T input_value): m_id(id), m_type(type), m_name(t_name) {
        setValue(input_value);
        m_timestamp = getTimestamp();
    };

    void clear() {

    }

    template <typename T>
    void setValue(T input_value) {
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

    uint64_t getTimestamp() {
        std::chrono::_V2::system_clock::duration time = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(time).count();
    }

    uint32_t m_id;
    std::string m_name;
    uint64_t m_timestamp;
    Type m_type;
    ValueType m_value;
};

struct SharedMemoryData {
    pthread_mutex_t m_lock;
    uint32_t m_arr_count;
    Record m_arr[0];
};

class SharedMemory {
public:
    SharedMemory(const std::string &t_name, int t_entity_num): m_name(t_name) {
        m_shm_fd = shm_open(m_name.data(), O_CREAT | O_EXCL | O_RDWR, 0666);
        if (m_shm_fd < 0) {
            throw std::runtime_error("Shared memory with this name already exists");
        }

        m_memory_size = t_entity_num * sizeof(Record) + sizeof(SharedMemoryData);

        ftruncate(m_shm_fd, m_memory_size);

        m_data_ptr = (SharedMemoryData*)mmap(0, m_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_shm_fd, 0);
        m_data_ptr->m_arr_count = t_entity_num;
        m_mutex.init(m_data_ptr->m_lock);

        for (size_t i = 0; i < t_entity_num; i++) {
            m_data_ptr->m_arr[i].clear();
           std::cout << m_data_ptr->m_arr[i].m_id;
        }
    }
    
    void addRecord(const Record &t_record) {
        if (m_arr_size > m_data_ptr->m_arr_count) {
            m_mutex.lock(m_data_ptr->m_lock);

            m_data_ptr->m_arr[m_data_ptr->m_arr_count] = t_record;
            m_data_ptr->m_arr_count++;

            m_mutex.unlock(m_data_ptr->m_lock);
        }
    }

    void getRecords() {
        if (m_data_ptr->m_arr_count > 0) {
            m_mutex.lock(m_data_ptr->m_lock);
            for (size_t i = 0; i < m_data_ptr->m_arr_count; i++) {
                std::cout << &m_data_ptr->m_arr[i] << std::endl;
            }
            m_mutex.unlock(m_data_ptr->m_lock);
        }
    }

    ~SharedMemory() {
        shm_unlink(m_name.data());
    }

private:
    int m_shm_fd;
    SharedMemoryData* m_data_ptr;
    std::string m_name;
    int m_arr_size;
    int m_memory_size;
    int m_elem_size = sizeof(SharedMemoryData);
    SharedMemoryMutex m_mutex;
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