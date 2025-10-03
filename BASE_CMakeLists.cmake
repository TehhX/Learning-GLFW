# This file is to include in all individual project CMakeLists.txt files. This should not actually be used to build anything.

# When sharing resultant exec with other computers, defining BASE_STATIC as -static will make sure all relevant libs are compiled into the exec itself.
# set(BASE_STATIC -static) # Uncomment for static exe

# Standard of C++17 required:
set(CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Standard of C17 required:
set(C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Individual projects must specify ${SRC} with the names of all source files.
list(TRANSFORM SRC PREPEND "src/")

# Create executable named after the project name using files from SRC:
if(WIN32)
    # The WIN32 in the middle specifies the subsystem as Window, meaning that a new CMD window won't be spawned in addition to the main window.
    add_executable(${PROJECT_NAME} WIN32 ${SRC})
else()
    add_executable(${PROJECT_NAME} ${SRC})
endif()

# Get root folder "Learning-GLFW":
cmake_path(GET CMAKE_SOURCE_DIR PARENT_PATH ROOT)

# Attempt to find GLFW version >= 3.0.0 quietly.
find_package(glfw3 3 QUIET)
if (glfw3_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${BASE_STATIC})
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT}/libs/libglfw3.a ${BASE_STATIC})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${ROOT}/libs/glad.a ${BASE_STATIC})
target_include_directories(${PROJECT_NAME} PRIVATE ${ROOT}/include/)

# Link math library ("libm", lib presumed thus just 'm') for sin, pow etc.
target_link_libraries(${PROJECT_NAME} PRIVATE m ${BASE_STATIC})
