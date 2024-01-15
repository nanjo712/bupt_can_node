cmake_minimum_required(VERSION 3.15)

project(bupt_can VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use FindThreads to find the pthread library
find_package(Threads REQUIRED)

# Define the bupt_can library
add_library(${PROJECT_NAME}
    STATIC
    src/bupt_can.cpp
    include/bupt_can/bupt_can.h
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    -O2
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Threads::Threads
)

# Define the bupt_can_node executable
add_executable(${PROJECT_NAME}_node
    test/socket_can_test.cpp
)

target_link_libraries(${PROJECT_NAME}_node
    PRIVATE
    ${PROJECT_NAME}
)

# Install the library and headers
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Export the package for use from the build-tree
export(EXPORT ${PROJECT_NAME}-targets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
    NAMESPACE ${PROJECT_NAME}::
)

# Register package with CMake user package registry
export(PACKAGE ${PROJECT_NAME})

# Create a ConfigVersion file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY SameMajorVersion
)

# Create a Config file
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the Config and ConfigVersion files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)

# Install the export set
install(EXPORT ${PROJECT_NAME}-targets
    FILE ${PROJECT_NAME}-targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)
