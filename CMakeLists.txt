cmake_minimum_required(VERSION 3.22)

project(dpp-bot VERSION 1.0 DESCRIPTION "A discord bot")
     
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
    
aux_source_directory(src SOURCES)

add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

find_package(DPP REQUIRED)   
find_package(SQLite3 REQUIRED)

target_link_libraries(${PROJECT_NAME} ${DPP_LIBRARIES} ${SQLite3_LIBRARIES})

target_include_directories(
    ${PROJECT_NAME} PRIVATE 
        deps 
        include 
        ${DPP_INCLUDE_DIR} 
        ${SQLite3_INCLUDE_DIRS}
)
     
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)