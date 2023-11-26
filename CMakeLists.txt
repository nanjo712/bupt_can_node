cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_C_COMPILER "/usr/bin/gcc")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(bupt_can C CXX)

include_directories(inc)

set(
    SOURCES
    src/bupt_can.cpp
)

set(
    HEADERS
    include/bupt_can_node/bupt_can.h
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} pthread)

add_executable(${PROJECT_NAME}_node example/socket_can_test.cpp)
target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME})