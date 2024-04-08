cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME sort CXX)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DLLVM_ENABLE_PROJECTS=lld")

project(${PROJECT_NAME})

add_subdirectory(external/qcustomplot)

add_subdirectory(src/algoritms)
add_subdirectory(src/widgets)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}.widgets)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
