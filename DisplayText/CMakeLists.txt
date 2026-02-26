cmake_minimum_required(VERSION 3.28)
project(DisplayText LANGUAGES C VERSION 0.0.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SRC
    main.c
)

include("../BASE_CMakeLists.cmake")

# Create symlink in binary directory to FreeMono.otf if it's not there.
if (NOT EXISTS "${PROJECT_BINARY_DIR}/FreeMono.otf")
    if (UNIX)
        execute_process(COMMAND ln -s "${PROJECT_SOURCE_DIR}/FreeMono.otf" "${PROJECT_BINARY_DIR}/")
    elseif (WIN32)
        message(FATAL_ERROR "TODO: Create symlink on Windows. Untested version below, remove this line if it works.")
        execute_process(COMMAND mklink /D "${PROJECT_BINARY_DIR}/" "${PROJECT_SOURCE_DIR}/FreeMono.otf")
    else ()
        message(FATAL_ERROR "Unsupported OS.")
    endif ()
endif ()

if (UNIX)
    # Need to include the freetype2 include folder for some reason.
    target_include_directories(DisplayText PRIVATE /usr/include/freetype2/)
else ()
    message(FATAL_ERROR "On non-UNIX OS, need to know where freetype2 is.")
endif ()

target_link_libraries(DisplayText PRIVATE freetype)
