cmake_minimum_required(VERSION 3.10)
project(backend)

set(CMAKE_CXX_STANDARD 17)

# Explicitly list your source directories
file(GLOB SOURCES
    "${CMAKE_SOURCE_DIR}/main.cpp"
    "${CMAKE_SOURCE_DIR}/anonymous-pipes/*.cpp"
    "${CMAKE_SOURCE_DIR}/local-sockets/*.cpp"
    "${CMAKE_SOURCE_DIR}/shared-memory/*.cpp"
)

add_executable(backend ${SOURCES})
