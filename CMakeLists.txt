cmake_minimum_required(VERSION 3.25)

set(CMAKE_C_COMPILER    "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER  "/usr/bin/g++")

project(test)

set(CMAKE_CXX_STANDARD 17)

add_executable(test main.cpp lib/tinyxml2.h lib/tinyxml2.cpp)
