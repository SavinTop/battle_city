#!/bin/sh
conan install . -if conan -b missing -s build_type=$1
cmake -S . -B build -DCMAKE_BUILD_TYPE=$1
cmake --build build