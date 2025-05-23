cmake_minimum_required(VERSION 3.30)
project(6502_Emulator)

set(CMAKE_CXX_STANDARD 20)

add_executable(app main.cpp)

target_link_libraries(app PRIVATE 6502_Library)

target_include_directories(app PRIVATE ${CMAKE_SOURCE_DIR}/6502_Library)