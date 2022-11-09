# Open Addressed Hash Map

Project aims to build a drop-in replacement for std::unordered_map, being an open-addressed hash map built for use in Optiver, where each node contains a key and a pointer to a value, as described in a CppCon 2017 talk by Carl Cook.

[Here's the Medium post](https://medium.com/@nhzaci/writing-drop-in-unordered-map-implementations-c-c2c76a2bf1bc) elaborating more on the motivation and reasoning behind building such a hash map and how it would be more cache-efficient compared to a hash map implemented with chaining.

# Build and Running

## Requirements

- C++ 20
- CMake v3.23

## Build

### Using the build script

```bash
chmod u+x build.sh && ./build.sh
```

### Using CLI

```bash
# remove existing build, make build directory and cd inside
rm -rf build && mkdir build && cd build

# init cmake from CMakeLists in project root
cmake -DCMAKEBUILD_TYPE=Relase ..

# build
cmake --build .
```

# TODO

- [X] Initial implementation with key features, find, insert, clear
- [X] Expand the map when it reaches its load factor
- [X] Benchmark it compared to std::unordered_map
- [] Profile for memory leaks
- [] Implementing erase and emplace
- [] Write concepts for Probe and static_assert in map impl
- [] Make containers allocator-aware
- [] Optimizations for rvalue references

| Methods         | Impl? |
| --------------- | ----- |
| empty           | Y     |
| size            | Y     |
| max_size        | Y     |
| clear           | Y     |
| find            | Y     |
| insert          | Y     |
| emplace         | N     |
| erase           | N     |
| swap            | N     |
| at              | Y     |
| operator[]      | Y     |
| count           | Y     |
| find            | Y     |
| contains        | Y     |
| load_factor     | Y     |
| max_load_factor | Y     |
| rehash          | N     |
| reserve         | N     |
| hash_function   | Y     |
| key_eq          | Y     |

# References:

- [CppCon 2017 "When a microsecond is an eternity" - Carl Cook](https://youtu.be/NH1Tta7purM)