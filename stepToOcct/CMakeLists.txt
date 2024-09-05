cmake_minimum_required(VERSION 3.5.0)
project(stepToOcct VERSION 0.1.0 LANGUAGES CXX)

add_executable(stepToOcct main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCASCADE REQUIRED)

include_directories(${OpenCASCADE_INCLUDE_DIR})

target_link_libraries(stepToOcct ${OpenCASCADE_LIBRARIES})