cmake_minimum_required(VERSION 3.31)
project(6502_Emulator)

set(CMAKE_CXX_STANDARD 20)

add_library(6502_Library STATIC
        emulator_6502.cpp
        emulator_6502.h
)

target_include_directories(6502_Library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
