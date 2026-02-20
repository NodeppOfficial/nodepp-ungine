cmake_minimum_required(VERSION 3.10)
project(nodepp-ungine VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(UNGINE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 1. Search for Nodepp Core
if (NOT TARGET nodepp)
    find_package(nodepp REQUIRED)
endif()

# 2. Search for Nodepp-raylib
if (NOT TARGET nodepp-raylib)
    find_package(nodepp-raylib REQUIRED)
    if(NOT nodepp-raylib_FOUND)
        message(FATAL_ERROR "nodepp-raylib not found.")
    endif()
endif()

# 3. Define the interface target
# Use 'add_library' without FetchContent for local interface targets
add_library( nodepp-ungine INTERFACE )

target_include_directories(nodepp-ungine INTERFACE ${UNGINE_INCLUDE_DIR})

# 4. Link dependencies
target_link_libraries( nodepp-ungine INTERFACE nodepp nodepp-raylib )