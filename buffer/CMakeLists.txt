cmake_minimum_required(VERSION 3.8)
project(buffer)

aux_source_directory(. srcs)

add_library(${PROJECT_NAME} SHARED ${srcs})
target_include_directories(${PROJECT_NAME} INTERFACE .)
target_link_libraries(${PROJECT_NAME} PRIVATE logger)