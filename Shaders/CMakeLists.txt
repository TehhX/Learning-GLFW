cmake_minimum_required(VERSION 3.28)
project(Shaders LANGUAGES C)

set(SRC
    main.c
    generated_shaders.c
)

include("../BASE_CMakeLists.txt")
