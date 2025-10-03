cmake_minimum_required(VERSION 3.28)
project(Shaders LANGUAGES C)

# Calculate the FPS each frame.
add_compile_definitions(CALCULATE_FPS)

set(SRC
    main.c
    gensh.c
)

include("../BASE_CMakeLists.cmake")
