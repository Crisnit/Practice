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
    SharedMemoryMutex() {
        pthread_mutexattr_t psharedm;
        pthread_condattr_t psharedc;

        pthread_mutexattr_init(&psharedm);
        pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);

        pthread_condattr_init(&psharedc);
        pthread_condattr_setpshared(&psharedc, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&m_lock, &psharedm);
        pthread_cond_init(&nonzero,&psharedc);
    }
    void lock() {
        pthread_mutex_lock(&m_lock);
    }
    void unlock() {
        pthread_mutex_unlock(&m_lock);
    }
    pthread_mutex_t m_lock;
    pthread_cond_t nonzero;
};


struct Record {
public:
    union ValueType {
        float fl;
        double db;
        int32_t i;
        uint64_t ui;
        char ch[32];
    };

    enum Type {
        float_type, 
        double_type, 
        int_type, 
        uint_type, 
        string_type
    };

    template <typename T> Record(uint32_t id, std::string &t_name, Type type, T &input_value)
        : m_id(id), m_type(type) {
        std::strncpy(m_name, t_name.c_str(), 32);
        m_name [31] = '\0';
        setDecimalValue(input_value);
        m_timestamp = getTimestamp();
    };

    Record(uint32_t id, std::string &t_name, Type type, std::string &input_value)
        : m_id(id), m_type(type) {
        std::strncpy(m_name, t_name.c_str(), 32);
        m_name [31] = '\0';
        setStrValue(input_value);
        m_timestamp = getTimestamp();
    };

    void clear() { }

    template <typename T>
    void setDecimalValue(T &input_value) {
        switch (m_type) {
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
        }
    }

    void setStrValue(std::string &t_value) {
        std::strncpy(m_value.ch, t_value.c_str(), 32);
        m_value.ch[31] = '\0';
    }

    template <typename T>
    void changeValue(Type t_type, T &input_value) {
        m_type = t_type;
        setDecimalValue(input_value);
        m_timestamp = getTimestamp();
    }

    void changeValue(Type t_type, std::string &input_value) {
        m_type = t_type;
        setStrValue(input_value);
        m_timestamp = getTimestamp();
    }

    std::string getDateTime() const {
        int microseconds = m_timestamp%1000000;

        char buff[128];

        std::chrono::duration<uint64_t, std::micro> dur(m_timestamp);
        auto tp = std::chrono::system_clock::time_point(
        std::chrono::duration_cast<std::chrono::system_clock::duration>(dur));
        std::time_t in_time_t = std::chrono::system_clock::to_time_t(tp);
        strftime(buff, 128, "%Y-%m-%d %H:%M:%S", localtime(&in_time_t));
        std::string resDate(buff);
        resDate += "." + std::to_string(microseconds);
        return resDate;
    }

    uint64_t getTimestamp() {
        std::chrono::_V2::system_clock::duration time = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(time).count();
    }

    uint32_t m_id;
    char m_name[32];
    uint64_t m_timestamp;
    Type m_type;
    ValueType m_value;
};

std::ostream& operator << (std::ostream &os, const Record &rec) {
    os << rec.m_id << " " << std::string(rec.m_name) << " " << rec.getDateTime() << " ";

    switch (rec.m_type) 
    {
    case Record::Type::float_type:
        return os << "float" << " " << rec.m_value.fl;
        break;
    case Record::Type::double_type:
        return os << "double" << " " << rec.m_value.db;
        break;
    case Record::Type::int_type:
        return os << "integer" << " " << rec.m_value.i;
        break;
    case Record::Type::uint_type:
        return os << "unsigned_int" << " " << rec.m_value.ui;
        break;
    case Record::Type::string_type:
        return os << "string" << " " << std::string(rec.m_value.ch);
        break;
    }
    return os;
}

struct SharedMemoryData {
    SharedMemoryMutex m_mutex;
    uint32_t m_arr_count = 0;
    uint32_t m_arr_capacity;
    Record m_arr[0];
};

class SharedMemory {
public:
    void setName(const std::string &t_name) {
        m_name = t_name;
    }

    void setMemSize(int t_memory_size) {
        m_memory_size = t_memory_size;
    }

    SharedMemory() {}

    void addRecord(const Record &t_record) {
        m_data_ptr->m_mutex.lock();
        if (m_data_ptr->m_arr_capacity > m_data_ptr->m_arr_count) 
        {
            m_data_ptr->m_arr[m_data_ptr->m_arr_count] = t_record;
            m_data_ptr->m_arr_count++;

            m_data_ptr->m_mutex.unlock();
        }
        else 
        {
            m_data_ptr->m_mutex.unlock();
            throw std::out_of_range("Array is full");
        }
    }

    void getRecords() {
        m_data_ptr->m_mutex.lock();
        if (m_data_ptr->m_arr_count > 0) 
        {
            for (size_t i = 0; i < m_data_ptr->m_arr_count; i++) 
            {
                std::cout << m_data_ptr->m_arr[i] << std::endl;
            }
        }
        m_data_ptr->m_mutex.unlock();
    }

    void getRecordById(int t_id) {
        m_data_ptr->m_mutex.lock();
        if (m_data_ptr->m_arr_count > 0) 
        {

            for (size_t i = 0; i < m_data_ptr->m_arr_count; i++) 
            {
                if (m_data_ptr->m_arr[i].m_id == t_id)
                {
                    std::cout << m_data_ptr->m_arr[i] << std::endl;
                    m_data_ptr->m_mutex.unlock();
                    return;
                }              
            }
            std::cout << "No record found" << std::endl;
        }
        m_data_ptr->m_mutex.unlock();
    }
    template <typename T>
    void changeRecordValue(int t_id, Record::Type t_type, T t_value) {
        m_data_ptr->m_mutex.lock();
        if (m_data_ptr->m_arr_count > 0) 
        {

            for (size_t i = 0; i < m_data_ptr->m_arr_count; i++) 
            {
                if (m_data_ptr->m_arr[i].m_id == t_id)
                {
                    m_data_ptr->m_arr[i].changeValue(t_type, t_value);
                    std::cout << m_data_ptr->m_arr[i]<< std::endl;
                    m_data_ptr->m_mutex.unlock();
                    return;
                }              
            }
            std::cout << "No record found" << std::endl;
        }
        m_data_ptr->m_mutex.unlock();
    }

    void getStats() {
        m_data_ptr->m_mutex.lock();

        std::cout << "Array capacity: " << m_data_ptr->m_arr_capacity << " " << "Array count: " << m_data_ptr->m_arr_count << std::endl;
        
        m_data_ptr->m_mutex.unlock();
    }

    void deleteSharedMemory() {
        shm_unlink(m_name.data());
    }

    int m_shm_fd;
    SharedMemoryData* m_data_ptr;
    std::string m_name;
    int m_memory_size;
    int m_elem_size = sizeof(SharedMemoryData);
};

class SharedMemoryBuilder {
public:
    virtual void buildName() = 0;
    virtual void buildMemSize() = 0;
    virtual SharedMemory getResult() = 0;
};

class ServerSharedMemoryBuilder : public SharedMemoryBuilder {
public:
    ServerSharedMemoryBuilder(const std::string &t_name, int t_entity_num): m_name(t_name), m_entity_num(t_entity_num) {
        // m_shared_memory = SharedMemory();
    }
    void buildName() override {
        m_shared_memory.setName(m_name);
    }

    void buildMemSize() override {
        //int memory_size = m_entity_num * sizeof(Record) + sizeof(SharedMemoryData);
        //m_shared_memory.setMemSize(memory_size);
        m_shared_memory.setMemSize(1*1024*1024);
    }
    SharedMemory getResult() override {
        m_shared_memory.m_shm_fd = shm_open(m_shared_memory.m_name.data(), O_CREAT | O_EXCL | O_RDWR, 0666);

        if (m_shared_memory.m_shm_fd < 0) 
        {
            shm_unlink(m_shared_memory.m_name.data());
            throw std::runtime_error("Shared memory with this name already exists");
        }

        ftruncate(m_shared_memory.m_shm_fd, m_shared_memory.m_memory_size);

        m_shared_memory.m_data_ptr = (SharedMemoryData*)mmap(0, m_shared_memory.m_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_shared_memory.m_shm_fd, 0);
        m_shared_memory.m_data_ptr->m_arr_capacity = m_entity_num;
        m_shared_memory.m_data_ptr->m_mutex = SharedMemoryMutex();

        for (size_t i = 0; i < m_entity_num; i++) 
        {
            m_shared_memory.m_data_ptr->m_arr[i].clear();
        }
        return m_shared_memory;
    }
    private:
    SharedMemory m_shared_memory;
    std::string m_name;
    int m_entity_num;
};

class ClientSharedMemoryBuilder : public SharedMemoryBuilder {
public:
    ClientSharedMemoryBuilder(const std::string &t_name): m_name(t_name) {}

    void buildName() override {
        m_shared_memory.setName(m_name);
    }

    void buildMemSize() override { }
    
    SharedMemory getResult() override {
        m_shared_memory.m_shm_fd = shm_open(m_shared_memory.m_name.data(), O_RDWR, 0666);
        if (m_shared_memory.m_shm_fd < 0) 
        {
             throw std::runtime_error("Shared memory does not exist");
        }
        const uint32_t shm_size = 1*1024*1024;
        m_shared_memory.m_data_ptr = (SharedMemoryData*)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_shared_memory.m_shm_fd, 0);

        return m_shared_memory;
    }
    private:
    SharedMemory m_shared_memory;
    std::string m_name;
};

class SharedMemoryDirector {
public:
    void buildSharedMemory(SharedMemoryBuilder &sharedMemoryBuilder) {
        sharedMemoryBuilder.buildName();
        sharedMemoryBuilder.buildMemSize();
    }
};