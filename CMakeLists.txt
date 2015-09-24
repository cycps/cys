cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(cys)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-std=c++1y -stdlib=libc++ \
-Wall -Wextra \
-g -O0 \
-fcolor-diagnostics -Wno-missing-braces")

add_library(cys Cypress.cxx Actuator.cxx Sensor.cxx)
add_executable(RotorSystem RotorSystem.cxx)
target_link_libraries(RotorSystem cys pthread)
