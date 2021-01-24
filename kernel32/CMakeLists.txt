find_package(nasm REQUIRED)
if(nasm_FOUND)
    message(STATUS "Found nasm ${NASM_VERSION_STRING} to build assembly primitives")
endif()

set(ENTRYPOINT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/EntryPoint.s)

add_custom_command(
        OUTPUT ${ENTRYPOINT_BIN}
        COMMAND ${NASM_EXECUTABLE} ARGS ${NASM_FLAGS} ${ENTRYPOINT_SRC} -o ${ENTRYPOINT_BIN}
        DEPENDS ${ENTRYPOINT_SRC}
        COMMENT "Creating ${ENTRYPOINT_BIN}")
add_custom_target(
        EntryPoint ALL
        DEPENDS ${ENTRYPOINT_BIN}
        VERBATIM)