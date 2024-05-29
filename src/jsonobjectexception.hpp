#ifndef JSONOBJECTEXCEPTION_HPP
#define JSONOBJECTEXCEPTION_HPP

#include <string>

namespace jsonmini {
    class JsonObjectException : public std::exception {
        friend class JsonObject;
    private:
        const std::string _what;
        const size_t _pos = 0;
        JsonObjectException(const char* msg, size_t pos);
        JsonObjectException(const char* msg);
        JsonObjectException(std::string msg, size_t pos);
    public:
        const char* what() const noexcept override;
        size_t pos() const noexcept;
    };
}
#endif
