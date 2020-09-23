cmake_minimum_required(VERSION 3.5)

project(logger)

aux_source_directory(. SRCS)

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_include_directories(${PROJECT_NAME} INTERFACE .)