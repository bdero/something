# Don't use cmake for this project - instead, run `make`.
# This dud `CMakeLists.txt` is just here to make CLion autocompletion work.

cmake_minimum_required(VERSION 3.6)
project(something)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.c)

include_directories(inc res /opt/toolchains/gen/m68k-elf/include)

add_executable(something ${SOURCE_FILES})
