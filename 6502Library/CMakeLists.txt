cmake_minimum_required(VERSION 3.16)
project(6502_Library)

set(CMAKE_CXX_STANDARD 17)


add_library(6502_Library
        src/emulator_6502.cpp
)

target_include_directories(6502_Library
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
