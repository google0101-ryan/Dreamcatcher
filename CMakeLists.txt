cmake_minimum_required(VERSION 3.20)

project(dc)

set(SOURCES cpu.cpp
            bus.cpp
            main.cpp)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(dc ${SOURCES})
