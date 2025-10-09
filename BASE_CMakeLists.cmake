# This file is to include in all individual sub-project CMakeLists.txt files. This should not actually be used to build anything.

if (MSVC)
    message(FATAL_ERROR "MSVC is not supported for various reasons, however contributions *towards* its support are welcome. For now, try GCC/GNU. Sorry!")
endif()

# Standard of C++17 required:
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Standard of C17 required:
set(C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Individual sub-projects must specify ${SRC} with the names of all source files.
list(TRANSFORM SRC PREPEND "src/")

# Create executable named after the sub-project name using files from SRC.
add_executable(${PROJECT_NAME} ${SRC})

# This specifies the subsystem as Windows, meaning that a new CMD window won't be spawned in addition to the main window. This only applies when Release is specified by config. WARNING! Console output is diabled when in Release config because of this.
if (WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
endif()

# Root folder (Assuming sub-project requiring step-out)
set(ROOT ${PROJECT_SOURCE_DIR}/..)

# Set include/libs directories:
target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/include/)
target_link_directories(${PROJECT_NAME} PRIVATE ${ROOT}/libs/)

# Link required libraries inside libs:
target_link_libraries(${PROJECT_NAME} PRIVATE glfw m cglm glad)
