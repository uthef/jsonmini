#include "jsonobject.hpp"

#include "jsonobjectexception.hpp"
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace jsonmini {
    const std::map<char, const char*> _OUTCC = {
        { '\n', "\\n" },
        { '\r', "\\r" },
        // { '\a', "\\\\a" },
        { '\t', "\\t" },
        { '\b', "\\b" },
        { '\f', "\\f" },
        // { '\v', "\\\\v" },
        // { '\e', "\\\\e" },
        { '"', "\\\""},
        { '\\', "\\\\"}
    };

    const std::map<char, char> _INCC = {
        { 'n', '\n' },
        { 'r', '\r' },
        // { 'a', '\a' },
        { 't', '\t' },
        { 'b', '\b' },
        { 'f', '\f' }
        // { 'v', '\v' },
        // { 'e', '\e' }
    };

    JsonObject::JsonObject() {
        _type = JSON_NULL;
    }

    JsonObject::JsonObject(double value) {
        _num = value;
        _type = JSON_NUMBER;
        _realNum = true;
    }

    JsonObject::JsonObject(long value) {
        _num = value;
        _type = JSON_NUMBER;
        _realNum = false;
    }

    JsonObject::JsonObject(bool value) {
        _bool = value;
        _type = JSON_BOOLEAN;
    }

    JsonObject::JsonObject(const char* value) : JsonObject(std::string(value)) {
        _str = value;
        _type = JSON_STRING;
    }

    JsonObject::JsonObject(std::string value) {
        _str = value;
        _type = JSON_STRING;
    }

    JsonObject::operator long() {
        return (long)_num;
    }

    JsonObject::operator double() {
        return _num;
    }

    JsonObject::operator bool() {
        return _bool;
    }

    JsonObject::operator const char *() {
        return _str.c_str();
    }

    JsonObject::operator std::string() {
        return _str;
    }

    JsonObject JsonObject::makeMap() {
        return JsonObject(JSON_MAP);
    }

    JsonObject JsonObject::makeArray() {
        return JsonObject(JSON_ARRAY);
    }

    JsonObject JsonObject::fromStr(std::string& str) {
        std::stringstream ss(str);
        JsonObject obj;

        obj << ss;

        return obj;
    }

    void JsonObject::remove(size_t index) {
        _arr.erase(_arr.begin() + index);
    }

    void JsonObject::setMinificationEnabled(bool value) {
        _min = value;
    }

    void JsonObject::setNullPropertyIgnoringEnabled(bool value) {
        _ignoreNull = value;
    }

    void JsonObject::clear() {
        switch (_type) {
            case JSON_STRING:
                _str.clear();
            break;
            case JSON_ARRAY:
                _arr.clear();
            break;
            case JSON_MAP:
                _map.clear();
            case JSON_NUMBER:
                _num = 0;
            case JSON_BOOLEAN:
                _bool = false;
            break;
        }
    }

    bool JsonObject::remove(std::string key) {
        return _map.erase(key) != 0;
    }

    bool JsonObject::hasKey(std::string key) {
        if (!isMap()) return false;

        return _map.find(key) != _map.end();
    }

    JsonObject& JsonObject::operator [](size_t index) {
        if (!isArray()) throw JsonObjectException("object cannot be used as array");

        while (_arr.size() < (index + 1)) {
            _arr.emplace_back();
        }

        return _arr[index];
    }

    JsonObject& JsonObject::operator [](std::string key) {
        if (!isMap()) throw JsonObjectException("object cannot be used as map");
        return _map[key];
    }

    JsonType JsonObject::type() const {
        return _type;
    }

    size_t JsonObject::size() const {
        switch (_type) {
            case JSON_ARRAY:
                return _arr.size();
            case JSON_MAP:
                return _map.size();
            case JSON_STRING:
                return _str.size();
            default:
                return 0;
        }
    }

    bool JsonObject::isArray() const {
        return _type == JSON_ARRAY;
    }

    bool JsonObject::isMap() const {
        return _type == JSON_MAP;
    }

    bool JsonObject::isString() const {
        return _type == JSON_STRING;
    }

    bool JsonObject::isNumber() const {
        return _type == JSON_NUMBER;
    }

    bool JsonObject::isBoolean() const {
        return _type == JSON_BOOLEAN;
    }

    bool JsonObject::isNull() const {
        return _type == JSON_NULL;
    }

    std::string JsonObject::str() {
        return _str;
    }

    bool JsonObject::boolean() {
        return _bool;
    }

    double JsonObject::number() {
        return _num;
    }

    long JsonObject::numberLong() {
        return (long)_num;
    }

    std::map<std::string, JsonObject>* JsonObject::map() {
        if (!isMap()) throw JsonObjectException("object is not a map");
        return &_map;
    }

    std::vector<JsonObject>* JsonObject::vector() {
        if (!isArray()) throw JsonObjectException("object is not an array");
        return &_arr;
    }

    void JsonObject::operator <<(const char* jsonStr) {
        std::stringstream ss(jsonStr);
        *this << ss;
    }

    // deserialization function
    void JsonObject::operator <<(std::istream& stream) {
        clear();

        if (_pdepth == 0) throw JsonObjectException("initial depth must not be equal to 0");

        size_t charSize;
        bool readskip = _lchar != '\0';
        bool drop = false;

        while (stream.peek() != EOF) {
            if (!readskip) {
                stream.get(_lchar);
                _pos++;
            }
            else readskip = false;

            size_t charSize = utf8CharSize(_lchar);

            if (charSize == 0) throw JsonObjectException("invalid utf-8 byte (data corruption)", _pos - 1);
            if (std::isspace(_lchar)) continue;

            // number deserialization
            if ((isDigit(_lchar) || _lchar == '-') && !_fin) {
                std::string numStr;
                numStr.push_back(_lchar);
                size_t begin = _pos;

                bool period = false,
                    neg = (_lchar == '-'),
                    e = false,
                    eSign = false,
                    numAfterE = false,
                    mbreak = false;

                while (stream.peek() != EOF) {
                    stream.get(_lchar);
                    _pos++;

                    if (isDigit(_lchar)) {
                        if (numStr.front() == '0' && !period) throw JsonObjectException("leading zero is not allowed");
                        if (e) numAfterE = true;
                        numStr.push_back(_lchar);
                        continue;
                    }

                    if (_lchar == '.') {
                        if (period) throw JsonObjectException("period cannot be used twice", _pos - 1);
                        if (e) throw JsonObjectException("period cannot be used here", _pos - 1);

                        period = true;
                        numStr.push_back(_lchar);
                        continue;
                    }

                    if (_lchar == '-' || _lchar == '+') {
                        if (!e || (e && eSign) || numAfterE) throw JsonObjectException("sign cannot be used here", _pos - 1);
                        if (e) eSign = true;
                        numStr.push_back(_lchar);
                        continue;
                    }

                    if (_lchar == 'e') {
                        if (e) throw JsonObjectException("invalid number format", _pos - 1);
                        e = true;
                        numStr.push_back(_lchar);
                        continue;
                    }

                    drop = true;
                    break;
                }

                if ((e && !numAfterE) || numStr == "-" || numStr.back() == '.') throw JsonObjectException("number expected", _pos - 1);

                _type = JSON_NUMBER;
                _realNum = period;

                try {
                    _num = std::stod(numStr);
                }
                catch (std::out_of_range&) {
                    throw JsonObjectException("number out of range", begin);
                }

                readskip = true;
                _fin = true;

                if (_pdepth == 1) continue;
                else break;
            }

            // keyword deserialization
            if (isalpha(_lchar) && !_fin) {
                std::string kw;
                kw.push_back(_lchar);

                size_t beginPos = _pos - 1;

                while (stream.peek() != EOF) {
                    stream.get(_lchar);
                    _pos++;

                    if (std::isalpha(_lchar)) {
                        kw.push_back(_lchar);
                        continue;
                    }

                    drop = true;
                    break;
                }

                if (kw == "true") {
                    _type = JSON_BOOLEAN;
                    _bool = true;
                }
                else if (kw == "false") {
                    _type = JSON_BOOLEAN;
                    _bool = false;
                }
                else if (kw == "null") {
                    _type = JSON_NULL;
                }
                else throw JsonObjectException("unknown identifier starting", beginPos);

                readskip = true;
                _fin = true;
                if (_pdepth == 1) continue;
                else break;
            }

            // string deserialization
            if (_lchar == '"' && !_fin) {
                bool esc = false;
                size_t begin = _pos;
                std::string str;

                while (stream.peek() != EOF) {
                    stream.get(_lchar);
                    _pos++;

                    if (!esc && _lchar == '\\') {
                        esc = true;
                        continue;
                    }

                    size_t chrSize = utf8CharSize(_lchar);

                    if (chrSize == 0) throw JsonObjectException("invalid utf-8 byte (data corruption)", _pos - 1);

                    if (chrSize == 1) {
                        if (isControl(_lchar)) {
                            throw JsonObjectException("control character", _pos - 1);
                        }

                        if (!esc && _lchar == '"') {
                            drop = true;
                            break;
                        }
                        else if (esc) {
                            if (_lchar == '\\' || _lchar == '"') str.push_back(_lchar);
                            else if (_lchar == 'u') {
                                // throw JsonException("unicode CC is not implemented", _pos - 1);

                                std::string hex;

                                while (stream.peek() != EOF && hex.size() < 4) {
                                    stream.get(_lchar);

                                    if (!isHex(_lchar)) {
                                        throw JsonObjectException("invalid unicode character escape sequence", _pos - 1);
                                    }

                                    hex.push_back(_lchar);
                                }

                                if (hex.size() != 4) {
                                    throw JsonObjectException("invalid unicode character escape sequence", _pos - 1);
                                }

                                size_t seqSize;
                                char seq[4];
                                codeToByteSeq(std::stoi(hex, 0, 16), seqSize, seq);

                                for (int i = 0; i < seqSize; i++) {
                                    str.push_back(seq[i]);
                                }

                                _pos += hex.size();

                                esc = false;
                                continue;

                            }
                            else {
                                auto sub = _INCC.find(_lchar);

                                if (sub == _INCC.end()) throw JsonObjectException("illegal escape sequence", _pos);

                                str.push_back(sub->second);
                            }

                            esc = false;
                            continue;
                        }
                    }

                    str.push_back(_lchar);

                    for (size_t i = 1; i < chrSize; i++) {
                        stream.get(_lchar);
                        str.push_back(_lchar);
                    }
                }

                if (!drop) throw JsonObjectException("unclosed string", begin - 1);

                _type = JSON_STRING;
                _str = str;
                _fin = true;
                if (_pdepth == 1) continue;
                else break;
            }

            // array/map deserialization
            if ((_lchar == '[' || _lchar == '{') && !_fin) {
                char closeChar = (_lchar == '[' ? ']' : '}');

                bool isMap = (closeChar == '}'),
                    closed = false,
                    comma = true,
                    valExp = true;

                while (stream.peek() != EOF) {
                    stream.get(_lchar);
                    _pos++;

                    if (std::isspace(_lchar)) continue;

                    if (_lchar == ',') {
                        if (comma) throw JsonObjectException("redundant comma", _pos - 1);
                        comma = true;
                        continue;
                    }

                    if (_lchar == closeChar) {
                        closed = true;
                        drop = true;
                        break;
                    }

                    if (!comma) {
                        throw JsonObjectException("comma or closing bracket expected", _pos - 1);
                    }

                    JsonObject key(_pdepth + 1, _pos, _lchar);

                    if (isMap) {
                        key << stream;

                        if (!key.isString()) throw JsonObjectException("string value expected", _pos - 1);

                        _pos = key._pos;
                        _lchar = key._lchar;

                        while (stream.peek() != EOF) {
                            stream.get(_lchar);
                            _pos++;
                            if (std::isspace(_lchar)) continue;
                            if (_lchar == ':') break;

                            throw JsonObjectException("key separator expected", _pos - 1);
                        }

                        stream.get(_lchar);
                        _pos++;

                        if (_lchar == EOF || _lchar == '}') throw JsonObjectException("value expected", _pos - 1);
                    }

                    JsonObject value(_pdepth + 1, _pos, _lchar);
                    value << stream;

                    _pos = value._pos;

                    // auto _lchar initialization
                    _lchar = value._lchar;

                    value._pdepth = 1;
                    value._lchar = '\0';
                    value._pos = 0;
                    value._fin = false;

                    if (isMap) _map[key.str()] = value;
                    else _arr.push_back(value);

                    comma = false;

                    if (_lchar == closeChar && !value.isArray() && !value.isMap()) {
                        closed = true;
                        drop = true;
                        break;
                    }

                    if (_lchar == ',') {
                        comma = true;
                    }
                }

                const size_t size = isMap ? _map.size() : _arr.size();

                if (!closed) throw JsonObjectException("closing bracket expected", _pos);
                if (comma && size > 0) throw JsonObjectException("redundant comma", _pos - 1);

                _type = (isMap ? JSON_MAP : JSON_ARRAY);
                _fin = true;
                if (_pdepth == 1) continue;
                else break;
            }

            throw JsonObjectException("character is not allowed here", _pos - 1);
        }

        if (_pdepth == 1) {
            _pdepth = 1;
            _lchar = '\0';
            _pos = 0;
            _fin = false;
        }

        if (_fin && drop) {
            auto exception = JsonObjectException("unexpected character", _pos - 1);

            if (!std::isspace(_lchar) && _lchar != ',' && _lchar != ']' && _lchar != '}' && _lchar != '"' && _lchar != ':') {
                throw exception;
            }
        }
    }

    // serialization function
    void JsonObject::operator >>(std::ostream& stream) {
        if (_fdepth == 0) throw JsonObjectException("initial depth must not be equal to 0");

        switch (_type) {
            case JSON_NULL:
                stream << "null";
            break;
            case JSON_NUMBER:
            {
                std::stringstream ss;

                if (_realNum) {
                    ss << std::setprecision(15) << _num;
                }
                else {
                    ss << std::setprecision(19) << (long)_num;
                }

                stream << ss.str();
            }
            break;
            case JSON_BOOLEAN:
                stream << (_bool ? "true" : "false");
            break;
            case JSON_STRING:
                stream << '"';

                for (auto b : _str) {
                    size_t charSize = utf8CharSize(b);

                    if (charSize != 1) {
                        stream << b;
                        continue;
                    }

                    auto pair = _OUTCC.find(b);

                    if (pair != _OUTCC.end()) {
                        stream << pair->second;
                        continue;
                    }

                    if (b > 0x00 && b <= 0x1b) {
                        throw JsonObjectException("unsupported control character");
                    }

                    stream << b;
                }

                stream << '"';
            break;
            case JSON_MAP:
            case JSON_ARRAY:
            {
                bool isMap = (_type == JSON_MAP);
                size_t size = (isMap ? _map.size() : _arr.size());
                size_t keyc = 0;

                size_t outSize = _map.size();
                bool hasNewLine = false;

                stream << (isMap ? '{' : '[');

                if (!_min && !isMap && !_arr.empty()) {
                    stream << '\n';
                }

                auto arrIter = _arr.begin();
                auto mapIter = _map.begin();

                for (size_t i = 0; i < size; i++) {
                    const std::string* key = 0;
                    JsonObject* value = 0;

                    if (isMap) {
                        key = &mapIter->first;
                        value = &mapIter->second;

                        if (value->isNull() && _ignoreNull) {
                            outSize--;
                            mapIter++;
                            continue;
                        }

                        if (keyc > 0) {
                            stream << ",";
                            if (!_min) stream << '\n';
                        }

                        if (!hasNewLine && !_min) {
                            hasNewLine = true;
                            stream << '\n';
                        }

                        if (!_min) {
                            fillDepth(stream, _fdepth);
                        }

                        JsonObject(*key) >> stream;

                        stream << ":";
                        if (!_min) stream << " ";

                        mapIter++;
                        keyc++;
                    }
                    else {
                        value = &*arrIter;
                        arrIter++;

                        if (i > 0) {
                            stream << ",";
                            if (!_min) stream << '\n';
                        }

                        if (!_min) {
                            fillDepth(stream, _fdepth);
                        }
                    }

                    auto& obj = *value;

                    bool min = obj._min,
                        ignore = obj._ignoreNull;

                    obj._min = _min;
                    obj._ignoreNull = _ignoreNull;
                    obj._fdepth = _fdepth + 1;

                    obj >> stream;

                    obj._fdepth = 1;
                    obj._min = min;
                    obj._ignoreNull = ignore;
                }

                if (isMap) {
                    if (!_min && outSize != 0) {
                        stream << "\n";
                        fillDepth(stream, _fdepth - 1);
                    }
                }
                else {
                    if (!_min && !_arr.empty()) {
                        stream << "\n";
                        fillDepth(stream, _fdepth - 1);
                    }
                }

                stream << (isMap ? '}' : ']');
            }
            break;
        }
    }

    JsonObject::JsonObject(JsonType type) {
        _type = type;
    };

    JsonObject::JsonObject(unsigned int pdepth, size_t pos, char lchar) {
        _pdepth = pdepth;
        _pos = pos;
        _lchar = lchar;
    }

    void JsonObject::fillDepth(std::ostream& stream, unsigned int depth) {
        for (unsigned int i = 0; i < depth; i++) {
            stream << '\t';
        }
    }

    bool JsonObject::isDigit(char byte) {
        return byte >= '0' && byte <= '9';
    }

    bool JsonObject::isControl(char signedByte) {
        unsigned char byte = signedByte;
        return (byte <= 0x1f) || (byte >= 0x80 && byte <= 0x9f);
    }

    bool JsonObject::isHex(char byte) {
        return (byte >= '0' && byte <= '9') || (byte >= 'a' && byte <= 'f') || (byte >= 'A' && byte <= 'F');
    }

    size_t JsonObject::utf8CharSize(char signedByte) {
        unsigned char byte = signedByte;

        if ((byte >= 0x80 && byte <= 0xc0)) {
            // invalid byte
            return 0;
        }
        if (byte >= 0xf0) {
            return 4;
        }

        if (byte >= 0xe0) {
            return 3;
        }

        if (byte >= 0xc0) {
            return 2;
        }

        return 1;
    }

    void JsonObject::codeToByteSeq(int code, size_t& size, char* arr) {
        if (code <= 0x7f) {
            size = 1;
            arr[0] = code;
            return;
        }

        if (code <= 0x7ff) {
            size = 2;
            arr[0] = 0xc0 | (code >> 6);
            arr[1] = 0x80 | (code & 0x3f);
            return;
        }

        if (code <= 0xffff) {
            size = 3;
            arr[0] = 0xe0 | (code >> 12);
            arr[1] = 0x80 | ((code >> 6) & 0x3f);
            arr[2] = 0x80 | (code & 0x3f);
            return;
        }

        if (code <= 0x10ffff) {
            size = 4;

            arr[0] = 0xf0 | (code >> 18);
            arr[1] = 0x80 | ((code >> 12) & 0x3f);
            arr[2] = 0x80 | ((code >> 6) & 0x3f);
            arr[3] = 0x80 | (code & 0x3f);
            return;
        }
    }
}
