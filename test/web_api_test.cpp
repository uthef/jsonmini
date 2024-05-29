#include <jsonobject.hpp>
#include <cassert>
#include <iostream>
#include <fstream>
#include "httputils.cpp"

using namespace jsonmini;

const char* URLS[] = {
    "https://dummyjson.com/posts/search?q=love",
    "https://dummyjson.com/products",
    "http://universities.hipolabs.com/search?country=France"
};

int main() {
    std::cout << "=== Web API test ===" << std::endl;

    JsonObject obj;
    std::ofstream ofs;

    obj.setMinificationEnabled(false);

    size_t idx = 0;

    for (auto url : URLS) {
        std::cout << std::string(url);
        parseFromUrl(url, &obj);
        auto filename = "url_result_" + std::to_string(++idx) + ".json";

        ofs.open(filename);
        assert(ofs.is_open());

        obj >> ofs;
        ofs.close();

        std::cout << "\n\t" << "'" + filename + "'" << " has been written to the build directory" << std::endl;
    }

    std::cout << std::endl;

    return 0;
}


