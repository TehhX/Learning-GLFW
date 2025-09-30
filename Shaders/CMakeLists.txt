cmake_minimum_required(VERSION 3.28)
project(Shaders LANGUAGES C)

# Calculate the FPS each frame.
# add_compile_definitions(CALCULATE_FPS)

set(SRC
    main.c
    gensh.c # Shader files placed inside string literals by "embed_glsl.c".
)

include("../BASE_CMakeLists.cmake")
