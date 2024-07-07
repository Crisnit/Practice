#include <cstdint>
#include <cstring>
#include <chrono>

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
    template <typename T> Record(uint32_t id, char name[64], Type type, T input_value): id(id), type(type)
    {
        std::memcpy(Name, name, 64);
        setValue(input_value);
        timestamp = getTimestamp();
    };
private:
    template <typename T>
    void setValue(T input_value){
        switch (type)
        {
        case float_type:
            value.fl = input_value;
            break;
        case double_type:
            value.db = input_value;
            break;
        case int_type:
            value.i = input_value;
            break;
        case uint_type:
            value.ui = input_value;
            break;
        case char_type:
            std::memcpy(value.ch, input_value, 32);
            break;
        }
    }

    uint64_t getTimestamp(){
        std::chrono::_V2::system_clock::duration time = std::chrono::high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(time).count();
    }

    uint32_t id;
    char Name[64];
    uint64_t timestamp;
    Type type;
    ValueType value;
};

class SharedMemory{

private:
    char m_name[16];
    int m_arr_count;
    int m_memory_size;
    int m_elem_size = sizeof(Record);
};