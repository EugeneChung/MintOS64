add_executable(${IMAGE_MAKER_BIN} main.c)
add_custom_target(
        ImageMaker ALL
        DEPENDS ${IMAGE_MAKER_BIN})
