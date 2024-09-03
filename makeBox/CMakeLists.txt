cmake_minimum_required(VERSION 3.5)
project(makeBox)

# 실행파일 설정
add_executable(makeBox main.cpp)

# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

# C++ 표준 설정
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# OpenCASCADE 경로 수동 설정
# set(OpenCASCADE_DIR "/path/opencascade")

# 라이브러리 찾기
find_package(OpenCASCADE REQUIRED)

# 헤더파일 경로 설정
include_directories(${OpenCASCADE_INCLUDE_DIR})

# 링크할 라이브러리 경로
target_link_libraries(makeBox ${OpenCASCADE_LIBRARIES})