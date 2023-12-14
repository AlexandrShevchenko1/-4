cmake_minimum_required(VERSION 3.25)
project(4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lpthread")

add_executable(4 main.cpp)
