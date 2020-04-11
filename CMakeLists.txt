cmake_minimum_required(VERSION 3.16)
project(mintos64 LANGUAGES C VERSION 0.1)

set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/target")

add_subdirectory(bootloader)