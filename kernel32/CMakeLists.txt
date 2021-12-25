find_package(nasm REQUIRED)
if(nasm_FOUND)
    message(STATUS "Found nasm ${NASM_VERSION_STRING} to build assembly primitives")
endif()

set(KERNEL32_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(KERNEL32_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KERNEL32_LDSCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts)
set(ENTRYPOINT_SRC ${KERNEL32_SRC_DIR}/EntryPoint.asm)
set(ENTRYPOINT_BIN EntryPoint.bin)
file(GLOB KERNEL32_HEADERS CONFIGURE_DEPENDS "${KERNEL32_INCLUDE_DIR}/*.h")

add_custom_command(
        OUTPUT ${ENTRYPOINT_BIN}
        COMMAND ${NASM_EXECUTABLE} ARGS ${ENTRYPOINT_SRC} -o ${ENTRYPOINT_BIN}
        DEPENDS ${ENTRYPOINT_SRC}
        COMMENT "Creating ${ENTRYPOINT_BIN}")
add_custom_target(
        EntryPoint ALL
        DEPENDS ${ENTRYPOINT_BIN}
        VERBATIM)

set(MODE_SWITCH_SRC ${KERNEL32_SRC_DIR}/ModeSwitch.asm)
set(MODE_SWITCH_OBJ ModeSwitch.o)
set(KERNEL32_SRCS main.c page.c)
set(KERNEL32_OBJS "")
set(KERNEL32_ELF main.elf)
set(KERNEL32_ELF_BIN kernel32.elf.bin)

foreach(SRC_FILE IN LISTS KERNEL32_SRCS)
    string(REGEX REPLACE "\\.[^.]*$" "" SRC_NAME ${SRC_FILE})
    set(OBJ_FILE ${SRC_NAME}.o)
    add_custom_command(
            OUTPUT ${OBJ_FILE}
            COMMAND ${X64_PC_LINUX_GCC} -I${KERNEL_INCLUDE_DIR} -I${KERNEL32_INCLUDE_DIR} -c -m32 -ffreestanding ${KERNEL32_SRC_DIR}/${SRC_FILE}
            DEPENDS ${KERNEL_HEADERS} ${KERNEL32_HEADERS}
            COMMENT "Creating freestanding object file for ${OBJ_FILE}")
    list(APPEND KERNEL32_OBJS ${OBJ_FILE})
endforeach()
add_custom_command(
        OUTPUT ${MODE_SWITCH_OBJ}
        COMMAND ${NASM_EXECUTABLE} ARGS -f elf32 ${MODE_SWITCH_SRC} -o ${MODE_SWITCH_OBJ}
        DEPENDS ${MODE_SWITCH_SRC}
        COMMENT "Creating ${MODE_SWITCH_OBJ}")
list(APPEND KERNEL32_OBJS ${MODE_SWITCH_OBJ})
add_custom_command(
        OUTPUT ${KERNEL32_ELF}
        COMMAND ${X64_PC_LINUX_LD} -melf_i386 -T ${KERNEL32_LDSCRIPTS_DIR}/elf_i386.x -nostdlib -e main -Ttext 0x10200 ${KERNEL32_OBJS}
                -o ${KERNEL32_ELF}
        DEPENDS ${KERNEL32_OBJS}
        COMMENT "Creating ${KERNEL32_ELF}")
add_custom_command(
        OUTPUT ${KERNEL32_ELF_BIN}
        COMMAND ${X64_PC_LINUX_OBJCOPY} -j .text -j .data -j .rodata -j .bss -S -O binary ${KERNEL32_ELF} ${KERNEL32_ELF_BIN}
        DEPENDS ${KERNEL32_ELF}
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
