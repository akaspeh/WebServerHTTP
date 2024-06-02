cmake_minimum_required(VERSION 3.23)
project(WebServerHTTP)

set(CMAKE_CXX_STANDARD 23)

add_executable(WebServerHTTP main.cpp)
target_link_libraries(WebServerHTTP PRIVATE ws2_32)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
set(SOURCE_FILES main.cpp)
set(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(your_project ${SOURCE_FILES})

target_link_libraries(your_project ${CMAKE_EXE_LINKER_FLAGS})