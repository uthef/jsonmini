#include <jsonobject.hpp>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace jsonmini;

int main() {
    auto obj = JsonObject();
    std::ifstream fs;

    fs.open(TYPES_JSON_PATH);
    assert(fs.is_open());

    obj << fs;

    std::cout << "=== Quick type test ===" << std::endl;
    obj >> std::cout;
    std::cout << std::endl << std::endl;

    return 0;
}

