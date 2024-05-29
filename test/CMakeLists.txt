cmake_minimum_required(VERSION 3.15)
project(quick_type_test)
project(complex_structure_test)
project(exception_test)
project(web_api_test)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(TEST_DATA_PATH ${CMAKE_SOURCE_DIR}/test/data)

add_definitions(-DTYPES_JSON_PATH="${TEST_DATA_PATH}/types.json")
add_definitions(-DPAGE_JSON_PATH="${TEST_DATA_PATH}/page.json")
add_definitions(-DMALFORMED_JSON_PATH="${TEST_DATA_PATH}/malformed_json.txt")

add_executable(quick_type_test quick_type_test.cpp)
add_executable(complex_structure_test complex_structure_test.cpp)
add_executable(exception_test exception_test.cpp)
add_executable(web_api_test web_api_test.cpp httputils.cpp)

find_package(CURL REQUIRED)

target_link_libraries(quick_type_test PRIVATE
    jsonmini
)

target_link_libraries(complex_structure_test PRIVATE
    jsonmini
)

target_link_libraries(exception_test PRIVATE
    jsonmini
)

target_link_libraries(web_api_test PRIVATE
    jsonmini
    CURL::libcurl
)
