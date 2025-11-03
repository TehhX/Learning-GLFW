cmake_minimum_required(VERSION 3.28)
project(ParticleSim LANGUAGES C VERSION 0.0.0)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SRC
    main.c
)

include("../BASE_CMakeLists.cmake")
