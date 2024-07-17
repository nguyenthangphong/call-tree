cmake_minimum_required(VERSION 3.5)

project(call-tree-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../form)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(GTest REQUIRED)

set(UIS
    ${CMAKE_CURRENT_SOURCE_DIR}/../form/calltree.ui
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/calltree.h
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/calltree.cpp
)

set(TESTS
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit_test/Test_GetFlag.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit_test/Test_GetMode.cpp
)

add_executable(run_tests ${TESTS} ${SOURCES} ${HEADERS} ${UIS})

set_target_properties(run_tests PROPERTIES AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../form)

target_link_libraries(run_tests GTest::gtest_main Qt${QT_VERSION_MAJOR}::Widgets gtest)

gtest_discover_tests(run_tests)
