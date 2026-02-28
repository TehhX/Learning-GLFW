cmake_minimum_required(VERSION 3.28)
project(Textures LANGUAGES C)

set(SRC
    main.c
    gensh.c
)

include("../BASE_CMakeLists.cmake")

# Create symlink in binary directory to bricks.jpg if it's not there.
if (NOT EXISTS "${PROJECT_BINARY_DIR}/res/bricks.jpg")
    if (UNIX)
        execute_process(COMMAND mkdir "${PROJECT_BINARY_DIR}/res/")
        execute_process(COMMAND ln -s "${PROJECT_SOURCE_DIR}/res/bricks.jpg" "${PROJECT_BINARY_DIR}/res/")
    elseif (WIN32)
        message(FATAL_ERROR "TODO: Create symlink on Windows. Untested version below, remove this line if it works.")
        # May not create parent folder res/
        execute_process(COMMAND mklink /D "${PROJECT_BINARY_DIR}/res/" "${PROJECT_SOURCE_DIR}/res/bricks.jpg")
    else ()
        message(FATAL_ERROR "Unsupported OS.")
    endif ()
endif ()

target_link_libraries(Textures PRIVATE m)
