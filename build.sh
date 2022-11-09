#!/usr/bin/env bash

# remove existing build, make build directory and cd inside
rm -rf build && mkdir build

# build into "build/" folder
cmake -DCMAKE_BUILD_TYPE=Release -S . -B "build"

# build Release
cmake --build "build" --config Release
