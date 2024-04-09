cmake_minimum_required(VERSION 3.22)

project(unit-tests LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake)

aux_source_directory(${CMAKE_SOURCE_DIR} SOURCES)
aux_source_directory(${CMAKE_SOURCE_DIR}/../src SOURCES)

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(GTest REQUIRED)
find_package(DPP REQUIRED)
find_package(SQLite3 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${GTest_INCLUDE_DIRS} 
    ${DPP_INCLUDE_DIR} 
    mocks 
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../deps
    ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} 
    GTest::gtest
    GTest::gmock 
    GTest::gtest_main 
    ${DPP_LIBRARIES}
    ${SQLite3_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)