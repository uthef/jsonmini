#ifndef JSONOBJECT_HPP
#define JSONOBJECT_HPP

#include <cstddef>
#include <vector>
#include <map>
#include <string>
#include "jsontype.hpp"

namespace jsonmini {
    class JsonObject {
    public:
        JsonObject();
        JsonObject(double value);
        JsonObject(long value);
        JsonObject(bool value);
        JsonObject(const char* value);
        JsonObject(std::string value);

        static JsonObject makeMap();
        static JsonObject makeArray();
        static JsonObject fromStr(std::string&);

        void remove(size_t index);
        void setMinificationEnabled(bool value);
        void setNullPropertyIgnoringEnabled(bool value);
        void clear();
        bool remove(std::string key);
        bool hasKey(std::string key);

        JsonObject& operator [](size_t index);
        JsonObject& operator [](std::string key);

        explicit operator double();
        explicit operator long();
        explicit operator bool();
        explicit operator const char*();
        explicit operator std::string();

        JsonType type() const;

        size_t size() const;

        bool isArray() const;
        bool isMap() const;
        bool isString() const;
        bool isNumber() const;
        bool isBoolean() const;
        bool isNull() const;

        std::string str();
        bool boolean();
        double number();
        long numberLong();

        std::map<std::string, JsonObject>* map();
        std::vector<JsonObject>* vector();

        // deserialization function
        void operator <<(const char* jsonSt);
        void operator <<(std::istream& stream);

        // serialization function
        void operator >>(std::ostream& stream);

    private:
        JsonType _type = JsonType::JSON_NULL;

        // formatting parameters
        bool _min = true;
        bool _ignoreNull = false;
        unsigned int _fdepth = 1;

        // serialization parameters
        unsigned int _pdepth = 1;
        char _lchar = '\0';
        size_t _pos = 0;
        bool _fin = false;

        // possible values
        std::map<std::string, JsonObject> _map;
        std::vector<JsonObject> _arr;
        std::string _str;
        double _num = 0;
        bool _realNum = false;
        bool _bool = false;

        JsonObject(JsonType type);
        JsonObject(unsigned int pdepth, size_t pos, char lchar);

        // utility functions
        static void fillDepth(std::ostream& stream, unsigned int depth);
        static bool isDigit(char byte);
        static bool isControl(char byte);
        static bool isHex(char byte);
        static void codeToByteSeq(int code, size_t& size, char* arr);
        static size_t utf8CharSize(char signedByte);
    };
};

#endif
