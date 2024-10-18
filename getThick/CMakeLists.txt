cmake_minimum_required(VERSION 3.5.0)
project(getThick VERSION 0.1.0 LANGUAGES C CXX)

add_executable(getThick main.cpp)

set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenCASCADE REQUIRED)

include_directories(${OpenCASCADE_INCLUDE_DIR})

target_link_libraries(getThick ${OpenCASCADE_LIBRARIES})