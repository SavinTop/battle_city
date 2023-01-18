cmake_minimum_required(VERSION 3.0.0)
project(battle_city VERSION 0.2.0)

set (CMAKE_CXX_STANDARD 17)

enable_testing()

include(${CMAKE_SOURCE_DIR}/conan/conanbuildinfo.cmake)
conan_basic_setup()

set(MESSAGE_SRC_LIST
src/msg/field_msg.cpp 
src/msg/serde/serialization.cpp)

add_library(message STATIC ${MESSAGE_SRC_LIST})
target_include_directories(message PUBLIC include)

add_executable(battle_city src/main.cpp)
target_link_libraries(battle_city PRIVATE message)

add_executable(basic_tests tests/basic_tests.cpp)
target_link_libraries(basic_tests PRIVATE message ${CONAN_LIBS})

add_test(NAME basic_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin COMMAND basic_tests)

