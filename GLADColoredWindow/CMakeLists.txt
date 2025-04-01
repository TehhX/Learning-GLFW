cmake_minimum_required(VERSION 3.28)
project(GLADColoredWindow LANGUAGES C CXX)

# Standard of C++17 required:
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get root folder "Learning-GLFW":
cmake_path(GET CMAKE_SOURCE_DIR PARENT_PATH ROOT)

# Source files.
set(SRC
    src/main.cpp
)

# Create executable named EmptyWindow using files from SRC:
add_executable(${PROJECT_NAME} ${SRC})

# Attempt to find GLFW version >= 3.0.0 quietly.
find_package(glfw3 3 QUIET)
if (glfw3_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT}/libs/libglfw3.a)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT}/libs/glad.a)
target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/include/)
