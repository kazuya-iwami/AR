cmake_minimum_required(VERSION 3.1)
project(server)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.cpp)

add_executable(server ${SOURCE_FILES} control.h control.cpp server.h)