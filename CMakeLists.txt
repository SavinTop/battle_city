cmake_minimum_required(VERSION 3.0.0)
project(battle_city VERSION 0.1.0)

set (CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(battle_city src/main.cpp src/field_msg.cpp)

