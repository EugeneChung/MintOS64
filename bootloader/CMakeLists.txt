find_package(nasm REQUIRED)
if(nasm_FOUND)
    message(STATUS "Found nasm ${NASM_VERSION_STRING} to build assembly primitives")
endif()

set(BOOTLOADER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/BootLoader.asm)
set(BOOTLOADER_BIN BootLoader.bin)
set(TEST_OS_IMAGE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/TestOSImage.asm)
set(TEST_OS_IMAGE_BIN TestOSImage.bin)
set(MINTOS64_BOOTLOADER_IMAGE Disk.img)

add_custom_command(
        OUTPUT ${BOOTLOADER_BIN}
        COMMAND ${NASM_EXECUTABLE} ARGS ${NASM_FLAGS} ${BOOTLOADER_SRC} -o ${BOOTLOADER_BIN}
        DEPENDS ${BOOTLOADER_SRC}
        COMMENT "Creating ${BOOTLOADER_BIN}")
add_custom_command(
        OUTPUT ${TEST_OS_IMAGE_BIN}
        COMMAND ${NASM_EXECUTABLE} ARGS ${NASM_FLAGS} ${TEST_OS_IMAGE_SRC} -o ${TEST_OS_IMAGE_BIN}
        DEPENDS ${TEST_OS_IMAGE_SRC}
        COMMENT "Creating ${TEST_OS_IMAGE_BIN}")
add_custom_target(
        BootLoader ALL
        DEPENDS ${BOOTLOADER_BIN} ${TEST_OS_IMAGE_BIN}
        VERBATIM)
add_custom_command(
        TARGET BootLoader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E cat
        ${CMAKE_CURRENT_BINARY_DIR}/${BOOTLOADER_BIN} ${CMAKE_CURRENT_BINARY_DIR}/${TEST_OS_IMAGE_BIN} >
        ${PROJECT_BINARY_DIR}/${MINTOS64_BOOTLOADER_IMAGE}
        COMMENT "Creating ${MINTOS64_BOOTLOADER_IMAGE}")
