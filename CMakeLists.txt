cmake_minimum_required(VERSION 3.15)
project(jsonmini)

add_subdirectory(test)

add_library(${PROJECT_NAME}
    src/jsonobjectexception.cpp
    src/jsonobject.cpp
)

enable_testing()
add_test(NAME quick_type_test COMMAND $<TARGET_FILE:quick_type_test>)
add_test(NAME complex_structure_test COMMAND $<TARGET_FILE:complex_structure_test>)
add_test(NAME exception_test COMMAND $<TARGET_FILE:exception_test>)
add_test(NAME web_api_test COMMAND $<TARGET_FILE:web_api_test>)
