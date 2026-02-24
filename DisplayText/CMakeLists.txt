cmake_minimum_required(VERSION 3.28)
project(DisplayText LANGUAGES C VERSION 0.0.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SRC
    main.c
)

include("../BASE_CMakeLists.cmake")

if (UNIX)
    # Need to include the freetype2 include folder for some reason.
    target_include_directories(DisplayText PRIVATE /usr/include/freetype2/)
else ()
    message(FATAL_ERROR "On non-UNIX OS, need to know where freetype2 is.")
endif ()

target_link_libraries(DisplayText PRIVATE freetype)
