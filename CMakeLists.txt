cmake_minimum_required(VERSION 3.28)
project(untitled)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    link_libraries(OpenMP::OpenMP_CXX)
endif()
add_executable(untitled main.cpp)
