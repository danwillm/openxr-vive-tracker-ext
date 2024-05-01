cmake_minimum_required(VERSION 3.22.1)

project(openxr_barebones)

add_subdirectory(lib/OpenXR-SDK)

add_executable(openxr_program src/main.cpp)

target_link_libraries(openxr_program PRIVATE openxr_loader)