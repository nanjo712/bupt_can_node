cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(can_node C CXX)

include_directories(inc)

set(
    SOURCES
    src/socket_can_test.cpp
    src/bupt_can.cpp
)

set(
    HEADERS
    inc/bupt_can.h
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} pthread)