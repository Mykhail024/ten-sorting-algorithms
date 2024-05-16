cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(PROJECT_NAME sort.widgets)

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)

project(${PROJECT_NAME})

find_package(Qt6 COMPONENTS Core Widgets LinguistTools REQUIRED)

set(PCH_HEADERS precompile.h)

set(TS_FILES
    ts/uk_UA.ts
    ts/en_US.ts
)

set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION "${CMAKE_BINARY_DIR}/bin/ts")

qt_create_translation(TSR_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

add_custom_target(translations ALL DEPENDS ${TSR_FILES})

add_executable(${PROJECT_NAME}
    main.cpp
    MainWindow.cpp
    ChangeLangDialog.cpp
    TableModel.cpp
    TableView.cpp
    RandomSourceModel.cpp
    GraphView.cpp
    StandardTestModel.cpp

    AbstractSourceModel.cpp
    AbstractTestModel.cpp

    SourceModelHandler.cpp
    AlgorithmHandler.cpp

    ${TSR_FILES}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_HEADERS})

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Widgets qcustomplot sort.algoritms)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
