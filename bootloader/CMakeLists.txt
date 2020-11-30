find_package(nasm REQUIRED)
if(nasm_FOUND)
    message(STATUS "Found nasm ${NASM_VERSION_STRING} to build assembly primitives")
endif()

set(ASM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/BootLoader.asm)
set(ASM_OUTPUT BootLoader.bin)
set(MINTOS64_BOOTLOADER_IMAGE Disk.img)

add_custom_command(
        OUTPUT ${ASM_OUTPUT}
        COMMAND ${NASM_EXECUTABLE} ARGS ${NASM_FLAGS} ${ASM_SRC} -o ${ASM_OUTPUT}
        DEPENDS ${ASM_SRC}
        COMMENT "Creating ${ASM_OUTPUT}"
)
add_custom_target(BootLoader ALL
        DEPENDS ${ASM_OUTPUT}

        # to make quotes printable, for example
        VERBATIM
)
add_custom_command(
        TARGET BootLoader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/${ASM_OUTPUT}
        ${PROJECT_BINARY_DIR}/${MINTOS64_BOOTLOADER_IMAGE})