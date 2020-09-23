cmake_minimum_required(VERSION 3.8)
project(multi-reactors-multi-threads)

aux_source_directory(. SRCS)

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_link_libraries(${PROJECT_NAME} PRIVATE logger)
target_link_libraries(${PROJECT_NAME} PRIVATE buffer)
target_include_directories(${PROJECT_NAME} INTERFACE .)