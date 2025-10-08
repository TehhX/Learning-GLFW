# This file is to include in all individual sub-project CMakeLists.txt files. This should not actually be used to build anything.

# Standard of C++17 required:
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Standard of C17 required:
set(C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Individual sub-projects must specify ${SRC} with the names of all source files.
list(TRANSFORM SRC PREPEND "src/")

# Create executable named after the sub-project name using files from SRC. The WIN32 in the middle specifies the subsystem as Windows, meaning that a new CMD window won't be spawned in addition to the main window. This only applies when on Windows, and is ignord on other OS's.
add_executable(${PROJECT_NAME} WIN32 ${SRC})

# Get root folder "Learning-GLFW":
cmake_path(GET CMAKE_SOURCE_DIR PARENT_PATH ROOT)

target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/include/)

target_link_directories(${PROJECT_NAME} PRIVATE ${ROOT}/libs/)

target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE m)
target_link_libraries(${PROJECT_NAME} PRIVATE cglm)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
