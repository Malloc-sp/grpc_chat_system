cmake_minimum_required(VERSION 3.17)
project(ChatSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) #需求C++17及以上
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(absl REQUIRED)
find_package(LogLib REQUIRED)

add_subdirectory(server)
add_subdirectory(client)

