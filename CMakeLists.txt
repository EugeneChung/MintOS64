cmake_minimum_required(VERSION 3.19)
project(mintos64 LANGUAGES C VERSION 0.1)

set(CMAKE_C_STANDARD 17)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/target")
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR})

set(MINTOS64_IMAGE MintOS64Disk.img)
set(BOOTLOADER_BIN BootLoader.bin)
set(ENTRYPOINT_BIN EntryPoint.bin)

add_subdirectory(bootloader)
add_subdirectory(kernel32)

add_custom_target(Kernel ALL VERBATIM)
add_dependencies(Kernel BootLoader EntryPoint)
add_custom_command(
        TARGET Kernel POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E cat
        ${CMAKE_CURRENT_BINARY_DIR}/bootloader/${BOOTLOADER_BIN} ${CMAKE_CURRENT_BINARY_DIR}/kernel32/${ENTRYPOINT_BIN} >
        ${PROJECT_BINARY_DIR}/${MINTOS64_IMAGE}
        COMMENT "Creating ${MINTOS64_IMAGE}")
