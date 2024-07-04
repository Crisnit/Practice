#include <cstdint>
#include <cstring>

union ValueType
    {
        float fl;
        double db;
        int32_t i;
        uint64_t ui;
        char ch[32];
    };

struct Record{
    public:
    enum Type {fl, db, i, ui, ch};
    Record(uint32_t id, char name[64], uint64_t timestamp, Type type, ValueType value): id(id), timestamp(timestamp), type(type)
    {
        std::memcpy(Name, name, 64);
    };
    private:
        uint32_t id;
        char Name[64];
        uint64_t timestamp;
        Type type;
        ValueType value;
};