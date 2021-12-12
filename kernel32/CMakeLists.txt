find_package(nasm REQUIRED)
if(nasm_FOUND)
    message(STATUS "Found nasm ${NASM_VERSION_STRING} to build assembly primitives")
endif()

set(KERNEL32_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(KERNEL32_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KERNEL32_LDSCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts)
set(ENTRYPOINT_SRC ${KERNEL32_SRC_DIR}/EntryPoint.asm)
set(ENTRYPOINT_BIN EntryPoint.bin)

add_custom_command(
        OUTPUT ${ENTRYPOINT_BIN}
        COMMAND ${NASM_EXECUTABLE} ARGS ${NASM_FLAGS} ${ENTRYPOINT_SRC} -o ${ENTRYPOINT_BIN}
        DEPENDS ${ENTRYPOINT_SRC}
        COMMENT "Creating ${ENTRYPOINT_BIN}")
add_custom_target(
        EntryPoint ALL
        DEPENDS ${ENTRYPOINT_BIN}
        VERBATIM)

set(KERNEL32_HEADERS ${KERNEL32_INCLUDE_DIR}/types.h)
set(KERNEL32_MAIN_SRC ${KERNEL32_SRC_DIR}/main.c)
set(KERNEL32_MAIN_OBJ main.o)
set(KERNEL32_MAIN_ELF main.elf)
set(KERNEL32_ELF_BIN kernel32.elf.bin)

add_custom_command(
        OUTPUT ${KERNEL32_MAIN_OBJ}
        COMMAND ${X64_PC_LINUX_GCC} -I${KERNEL32_INCLUDE_DIR} -c -m32 -ffreestanding ${KERNEL32_MAIN_SRC}
        DEPENDS ${KERNEL32_MAIN_SRC} ${KERNEL32_HEADERS}
        COMMENT "Creating freestanding ${KERNEL32_MAIN_OBJ}")
add_custom_command(
        OUTPUT ${KERNEL32_MAIN_ELF}
        COMMAND ${X64_PC_LINUX_LD} -melf_i386 -T ${KERNEL32_LDSCRIPTS_DIR}/elf_i386.x -nostdlib -e main -Ttext 0x10200 ${KERNEL32_MAIN_OBJ}
                -o ${KERNEL32_MAIN_ELF}
        DEPENDS ${KERNEL32_MAIN_OBJ}
        COMMENT "Creating ${KERNEL32_MAIN_ELF}")
add_custom_command(
        OUTPUT ${KERNEL32_ELF_BIN}
        COMMAND ${X64_PC_LINUX_OBJCOPY} -j .text -j .data -j .rodata -j .bss -S -O binary ${KERNEL32_MAIN_ELF} ${KERNEL32_ELF_BIN}
        DEPENDS ${KERNEL32_MAIN_ELF}
        COMMENT "Creating ${KERNEL32_ELF_BIN}")
add_custom_command(
        OUTPUT ${KERNEL32_BIN}
        COMMAND ${CMAKE_COMMAND} -E cat ${ENTRYPOINT_BIN} ${KERNEL32_ELF_BIN} > ${KERNEL32_BIN}
        DEPENDS ${ENTRYPOINT_BIN} ${KERNEL32_ELF_BIN}
        COMMENT "Creating ${KERNEL32_BIN}")
add_custom_target(
        Kernel32 ALL
        DEPENDS ${KERNEL32_BIN}
        VERBATIM)
