cmake_minimum_required(VERSION 3.20)
project(mintos64 LANGUAGES C VERSION 0.1)

set(CMAKE_C_STANDARD 11)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")
set(PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/target")
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR})

set(MINTOS64_IMAGE MintOS64Disk.img)
set(BOOTLOADER_BIN BootLoader.bin)
set(KERNEL32_BIN kernel32.bin)
set(IMAGE_MAKER_BIN mkimage)
set(X64_PC_LINUX_GCC /usr/local/cross/bin/x86_64-pc-linux-gcc)
set(X64_PC_LINUX_LD /usr/local/cross/bin/x86_64-pc-linux-ld)
set(X64_PC_LINUX_OBJCOPY /usr/local/cross/bin/x86_64-pc-linux-objcopy)

add_subdirectory(bootloader)
add_subdirectory(kernel32)
add_subdirectory(utility)

add_custom_target(Kernel ALL VERBATIM)
add_dependencies(Kernel BootLoader EntryPoint Kernel32 ImageMaker)
add_custom_command(
        TARGET Kernel POST_BUILD
        COMMAND utility/image_maker/mkimage -o ${PROJECT_BINARY_DIR}/${MINTOS64_IMAGE}
        ${CMAKE_CURRENT_BINARY_DIR}/bootloader/${BOOTLOADER_BIN} ${CMAKE_CURRENT_BINARY_DIR}/kernel32/${KERNEL32_BIN}
        COMMENT "Creating ${MINTOS64_IMAGE}")
