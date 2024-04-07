cmake_minimum_required(VERSION 3.22)

project(unit-tests LANGUAGES CXX)

aux_source_directory(${CMAKE_SOURCE_DIR} SOURCES)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(GTest REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${GTest_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} GTest::gtest GTest::gmock GTest::gtest_main)