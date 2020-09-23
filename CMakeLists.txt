cmake_minimum_required(VERSION 3.8)
project(reactor)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")
set(CMAKE_CXX_FLAGS "-fno-elide-constructors ${CMAKE_CXX_FLAGS}")
add_definitions(-DACSDK_DEBUG_LOG_ENABLED)

add_subdirectory(logger)
add_subdirectory(buffer)

add_subdirectory(single-reactor-single-thread)
add_subdirectory(single-reactor-single-thread/example)

add_subdirectory(single-reactor-multi-threads)
add_subdirectory(single-reactor-multi-threads/example)

add_subdirectory(multi-reactors-multi-threads)