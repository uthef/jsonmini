#include "jsonobjectexception.hpp"

namespace jsonmini {
    JsonObjectException::JsonObjectException(std::string msg, size_t pos)
        : _pos(pos), _what(msg + " at " + std::to_string(pos)) { }

    JsonObjectException::JsonObjectException(const char* msg) : _what(msg) { }

    JsonObjectException::JsonObjectException(const char* msg, size_t pos)
        : _pos(pos), _what(std::string(msg) + " at " + std::to_string(pos)) { }

    const char* JsonObjectException::what() const noexcept {
        return _what.c_str();
    }

    size_t JsonObjectException::pos() const noexcept {
        return _pos;
    }
}
