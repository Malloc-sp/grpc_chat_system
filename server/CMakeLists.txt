cmake_minimum_required(VERSION 3.10)
project(server)

add_executable(server src/main.cpp)
target_include_directories(server PRIVATE include)