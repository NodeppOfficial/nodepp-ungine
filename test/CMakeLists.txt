cmake_minimum_required(VERSION 3.16)
project(2D_SVG_RENDERER CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXECUTABLE_SUFFIX ".html")
    
include(FetchContent)
FetchContent_Declare(
    nodepp
    GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-wasm
    GIT_TAG          origin/main
    GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp)

include(FetchContent)
FetchContent_Declare(
    nodepp-raylib
    GIT_REPOSITORY   https://github.com/NodeppOfficial/nodepp-raylib
    GIT_TAG          origin/main
    GIT_PROGRESS     ON
)
FetchContent_MakeAvailable(nodepp-raylib)

add_executable( index ./main.cpp )
target_link_libraries( index PUBLIC nodepp nodepp-raylib)
target_include_directories( index PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")