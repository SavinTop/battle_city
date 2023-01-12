cmake_minimum_required(VERSION 3.0.0)
project(battle_city VERSION 0.1.1)

set (CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES
src/main.cpp
src/field_msg.cpp
src/msg_ser.cpp)

add_executable(battle_city ${SOURCES})

