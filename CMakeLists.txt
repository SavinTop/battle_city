cmake_minimum_required(VERSION 3.0.0)
project(battle_city VERSION 0.2.0)


set (CMAKE_CXX_STANDARD 17)

set(SRC_LIST
src/main.cpp 
src/field_msg.cpp 
src/serialization.cpp)

add_executable(battle_city src/main.cpp src/field_msg.cpp src/serialization.cpp)

target_include_directories(battle_city PUBLIC include)