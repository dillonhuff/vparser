cmake_minimum_required(VERSION 2.6)

project(vparser)

INCLUDE_DIRECTORIES(./src/)

SET(EXTRA_CXX_COMPILE_FLAGS "-std=c++11 -I./src -I./test -I/opt/local/include -O2 -Werror -Wall")

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${EXTRA_CXX_COMPILE_FLAGS}")

INCLUDE_DIRECTORIES(./src/)

SET(SRC_FILES ./src/tokenize.cpp
	      ./src/parse.cpp
              ./src/statement.cpp
              ./src/expression.cpp
              ./src/macro_def.cpp
              ./src/token.cpp)

SET(TEST_FILES ./test/tokenization_tests.cpp
               ./test/expression_parse_tests.cpp
	       ./test/parse_module_tests.cpp)


add_executable(all-tests ${TEST_FILES} ${SRC_FILES})
