cmake_minimum_required(VERSION 3.5.0)
project(readStep VERSION 0.1.0 LANGUAGES C CXX)

add_executable(readStep main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCASCADE REQUIRED)

include_directories(${OpenCASCADE_INCLUDE_DIR})

link_libraries(makeBox ${OpenCASCADE_LIBRARIES})