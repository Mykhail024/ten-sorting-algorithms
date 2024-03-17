cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME qcustomplot)

set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

find_package(Qt6 COMPONENTS Core Widgets PrintSupport REQUIRED)

add_library(${PROJECT_NAME} STATIC
    qcustomplot.h
    qcustomplot.cpp
)

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets Qt6::PrintSupport)
