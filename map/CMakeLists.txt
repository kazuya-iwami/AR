cmake_minimum_required(VERSION 2.8.4)
project(BDM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package( OpenCV REQUIRED )

add_executable(BDM sample.cpp)

target_link_libraries(BDM  ${OpenCV_LIBS})


