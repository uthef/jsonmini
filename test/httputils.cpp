#include <algorithm>
#include <cstring>
#include <curl/curl.h>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <stdio.h>
#include <jsonobject.hpp>
#include <cassert>
#include <iostream>

using namespace jsonmini;

struct Response {
    char* data = 0;
    size_t size = 0;
};

static size_t writeCallback(char* buffer, size_t size, size_t nmemb, void* ptr) {
    auto realsize = size * nmemb;
    auto response = (Response*)ptr;

    auto expPtr = (char*)std::realloc(response->data, response->size + realsize + 1);

    if (!expPtr) {
        throw std::runtime_error("out of memory");
    }

    response->data = expPtr;

    std::memcpy(expPtr + response->size, buffer, realsize);
    response->size += realsize;
    response->data[response->size] = 0;

    return realsize;
}

static void parseFromUrl(const char* url, JsonObject* obj) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL* curl = curl_easy_init();
    assert(curl);
    CURLcode code;
    Response resp;

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    code = curl_easy_perform(curl);

    // std::ofstream ofs;
    // ofs.open("libcurl.log", std::ios_base::trunc);
    // ofs << resp.data;
    // ofs.close();

    assert(code == CURLE_OK);

    struct curl_header* header = 0;
    struct curl_header* prev = 0;

    curl_header* contentType;

    *obj << resp.data;

    free(resp.data);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
