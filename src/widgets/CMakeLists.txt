cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME sort.widgets)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

find_package(Qt6 COMPONENTS Core Widgets Concurrent LinguistTools REQUIRED)

set(TS_FILES
    ts/uk_UA.ts
    ts/en_US.ts
)

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/bin/ts")

qt_create_translation(TSR_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

add_custom_target(translations ALL DEPENDS ${TSR_FILES})

file(GLOB ALGORITHM_THREADS "./algorithmThreads/*.cpp")

add_library(${PROJECT_NAME} STATIC
    MainWindow.cpp
    widgets.cpp
    visual.cpp
    algorithmThread.cpp
    visualWidget.cpp
    changeLangDialog.cpp
    model.cpp
    testThread.cpp

    ${ALGORITHM_THREADS}
    ${TSR_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets Qt6::Concurrent qcustomplot sort.algoritms)
