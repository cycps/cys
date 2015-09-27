cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(cys)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-std=c++1y -stdlib=libc++ \
-Wall -Wextra \
-g -O0 \
-fcolor-diagnostics -Wno-missing-braces")

include_directories(/usr/local/include)
include_directories(common)
include_directories(sim/lib)
include_directories(sim)
include_directories(control)
include_directories(control/lib)
link_directories(/usr/local/lib)



add_subdirectory(common)
add_subdirectory(sim)
add_subdirectory(control)
add_subdirectory(control/lib)
add_subdirectory(example)
