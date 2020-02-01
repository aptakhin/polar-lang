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
        lexer.cpp
        include/polar-lang/lexer.h
)
target_link_libraries(
    polar-parser-lib
        nlohmann_json::nlohmann_json
)

add_executable(
    polar-parser
        src/main.cpp
        include/polar-lang/common.h)
target_link_libraries(
    polar-parser
        polar-parser-lib
        nlohmann_json::nlohmann_json
)

add_executable(
    polar-test
        tests/test_json.cpp
        tests/test_lexer.cpp
)

target_link_libraries(
    polar-test
        polar-parser-lib
        nlohmann_json::nlohmann_json
        GTest::GTest
        GTest::Main
)

add_custom_command(
    OUTPUT lexer.cpp
    DEPENDS include/polar-lang/lexer.rl
    COMMAND ragel ${PROJECT_SOURCE_DIR}/include/polar-lang/lexer.rl
        -o ${CMAKE_BINARY_DIR}/lexer.cpp
    COMMENT "Ragel files"
)
