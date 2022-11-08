#!/usr/bin/env bash

# remove existing build, make build directory and cd inside
rm -rf build && mkdir build && cd build

# init cmake from CMakeLists in project root
cmake -DCMAKE_BUILD_TYPE=Release ..

# build
cmake --build .
