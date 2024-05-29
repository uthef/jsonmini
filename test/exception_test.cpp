#include <jsonobject.hpp>
#include <jsonobjectexception.hpp>
#include <fstream>
#include <cassert>
#include <iostream>

using namespace jsonmini;

int main() {
    std::cout << "=== Exception test == " << std::endl;
    std::ifstream ifs;
    ifs.open(MALFORMED_JSON_PATH);
    assert(ifs.is_open());

    const int cases = 7;

    char buff = '\0';
    int consn = 0;
    std::string jsonStrValues[cases];
    size_t idx = 0;

    while (ifs.get(buff) && buff != EOF) {
        if (buff == '\n' && ifs.peek() == '\r') {
            continue;
        }

        consn = ((buff == '\n') ? consn + 1 : 0);

        jsonStrValues[idx].push_back(buff);

        if (consn >= 2) {
            idx++;
        }
    }

    ifs.close();

    assert(idx == cases - 1);

    idx = 0;

    for (auto jsonStrValue : jsonStrValues) {
        try {
            JsonObject::fromStr(jsonStrValue);
        }
        catch (JsonObjectException& e) {
            std::cout << "Case " << (++idx) << " exception: ";
            std::cout << e.what() << std::endl;
            continue;
        }

        throw std::runtime_error("no JsonException has been caught!");
    }

    std::cout << std::endl;

    return 0;
}
