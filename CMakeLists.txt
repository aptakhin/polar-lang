cmake_minimum_required(VERSION 3.15)
project(polar-lang)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(
    polar-parser-lib
        src/parser.cpp
        include/polar-lang/parser.h
        src/ast.cpp
        scanner.cpp
        include/polar-lang/scanner.h
)
target_link_libraries(
    polar-parser-lib
        nlohmann_json::nlohmann_json
)

add_executable(
    polar-parser
        src/main.cpp
)

add_executable(
    polar-test
        tests/test_json.cpp
)

target_link_libraries(
    polar-test
        polar-parser-lib
        nlohmann_json::nlohmann_json
        GTest::GTest
        GTest::Main
)

add_custom_command(
    OUTPUT scanner.cpp
    DEPENDS include/polar-lang/scanner.rl
    COMMAND ragel ${PROJECT_SOURCE_DIR}/include/polar-lang/scanner.rl
        -o ${CMAKE_BINARY_DIR}/scanner.cpp
    COMMENT "Ragel files"
)
