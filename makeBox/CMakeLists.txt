cmake_minimum_required(VERSION 3.5.0)
project(makeSimpleSquare)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 라이브러리 경로 설정
find_package(OpenCASCADE REQUIRED)

# 소스 파일 추가
add_executable(makeSimpleSquare main.cpp)

# 링크 OpenCASCADE 라이브러리
target_link_libraries(makeSimpleSquare ${OpenCASCADE_LIBRARIES})
target_include_directories(makeSimpleSquare PRIVATE ${OpenCASCADE_INCLUDE_DIR})


