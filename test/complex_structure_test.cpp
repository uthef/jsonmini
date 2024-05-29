#include <jsonobject.hpp>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace jsonmini;

int main() {
    auto obj = JsonObject();
    std::ifstream ifs;

    ifs.open(PAGE_JSON_PATH);
    assert(ifs.is_open());

    obj << ifs;

    ifs.close();

    std::cout << "=== Complex structure test == " << std::endl;

    const size_t filecount = 4;

    std::string filenames[filecount] = {
        "page_output_with_null.json",
        "page_output_without_null.json",
        "page_output_with_null.min.json",
        "page_output_without_null.min.json"
    };

    bool props[filecount][2] = {
        { false, false },
        { false, true },
        { true, false},
        { true, true }
    };

    std::ofstream ofs;

    for (size_t i = 0; i < filecount; i++) {
        ofs.open(filenames[i], std::ios_base::trunc);
        assert(ofs.is_open());

        obj.setMinificationEnabled(props[i][0]);
        obj.setNullPropertyIgnoringEnabled(props[i][1]);

        obj >> ofs;

        std::cout << "'" + filenames[i] + "'" + " has been written to the build directory" << std::endl;

        ofs.close();
    }

    std::cout << std::endl;

    return 0;
}
