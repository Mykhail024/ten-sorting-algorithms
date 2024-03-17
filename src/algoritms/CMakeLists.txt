cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME sort.algoritms)

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC
    sort.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC .)
