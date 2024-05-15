cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME sort CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

project(${PROJECT_NAME})

add_subdirectory(external/qcustomplot)

add_subdirectory(src/algoritms)
add_subdirectory(src/widgets)
