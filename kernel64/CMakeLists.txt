find_package(nasm REQUIRED)
if(nasm_FOUND)
    message(STATUS "Found nasm ${NASM_VERSION_STRING} to build assembly primitives")
endif()

set(KERNEL64_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(KERNEL64_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(KERNEL64_LDSCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ldscripts)
set(ENTRYPOINT_SRC ${KERNEL64_SRC_DIR}/EntryPoint.asm)
set(ENTRYPOINT_OBJ EntryPoint.o)

set(KERNEL64_SRCS main.c)
set(KERNEL64_OBJS "")
set(KERNEL64_ELF main.elf)

add_custom_command(
        OUTPUT ${ENTRYPOINT_OBJ}
        COMMAND ${NASM_EXECUTABLE} ARGS -f elf64 ${ENTRYPOINT_SRC} -o ${ENTRYPOINT_OBJ}
        DEPENDS ${ENTRYPOINT_SRC}
        COMMENT "Creating ${ENTRYPOINT_OBJ}")
list(APPEND KERNEL64_OBJS ${ENTRYPOINT_OBJ})

foreach(SRC_FILE IN LISTS KERNEL64_SRCS)
    string(REGEX REPLACE "\\.[^.]*$" "" SRC_NAME ${SRC_FILE})
    set(OBJ_FILE ${SRC_NAME}.o)
    add_custom_command(
            OUTPUT ${OBJ_FILE}
            COMMAND ${X64_PC_LINUX_GCC} -I${KERNEL64_INCLUDE_DIR} -I${KERNEL_INCLUDE_DIR} -c -m64 -ffreestanding ${KERNEL64_SRC_DIR}/${SRC_FILE}
            DEPENDS ${KERNEL_HEADERS}
            COMMENT "Creating freestanding object file for ${OBJ_FILE}")
    list(APPEND KERNEL64_OBJS ${OBJ_FILE})
endforeach()
add_custom_command(
        OUTPUT ${KERNEL64_ELF}
        COMMAND ${X64_PC_LINUX_LD} -melf_x86_64 -T ${KERNEL64_LDSCRIPTS_DIR}/elf_x86_64.x -nostdlib -e main -Ttext 0x200000 ${KERNEL64_OBJS}
                -o ${KERNEL64_ELF}
        DEPENDS ${KERNEL64_OBJS}
        COMMENT "Creating ${KERNEL64_ELF}")
add_custom_command(
        OUTPUT ${KERNEL64_BIN}
        COMMAND ${X64_PC_LINUX_OBJCOPY} -j .text -j .data -j .rodata -j .bss -S -O binary ${KERNEL64_ELF} ${KERNEL64_BIN}
        DEPENDS ${KERNEL64_ELF}
        COMMENT "Creating ${KERNEL64_BIN}")
add_custom_target(
        Kernel64 ALL
        DEPENDS ${KERNEL64_BIN}
        VERBATIM)
