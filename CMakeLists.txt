cmake_minimum_required(VERSION 4.0)
project(SOProy)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)



add_executable(SOProy src/main.cpp
        include/processMemory/proMem.h
        src/proMem.cpp
)

target_include_directories(SOProy PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)