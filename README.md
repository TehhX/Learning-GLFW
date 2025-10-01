# Learning GLFW
A repo to learn the GLFW library, alongside GLAD and OpenGL. Made to work crossplatform with Windows/Linux.

## Repo Structure
The root folder contains the gitignore, README, and license files, along with several folders that have their own projects and CMake files within them. All of the folders are their own application, completely seperate from one another, and teach a different thing in GLFW, e.g. EmptyWindow shows the most basic GLFW program, an empty window. It doesn't come with include/ or libs/. Read the below section for more info on third-party libraries etc.

## Installing nesessary files
### Before running any project, make sure you have the following installed:
* GLFW3 - Through a package manager or by placing the relevant files under folder as shown in the below tree
* GLAD - By compiling a static library from their source and then placing the relevant files under this folder as shown in the below tree
* STB_IMAGE - By placing it under this folder as shown in the below tree

### Include/libs tree:
```
+---include
|   |   stb_image.h
|   |
|   +---GLAD
|   |       glad.h
|   |
|   +---GLFW
|   |       glfw3.h
|   |       glfw3native.h
|   |
|   \---KHR
|           khrplatform.h
|
+---libs
        glad.a
        libglfw3.a
```

## Compiling Any Example With CMake
Navigate to a subfolder e.g Shaders, run embed_glsl.c if there are any GLSL shader files (more info in [Shader Files](#shader-files)), build with cmake, and run.

## Compiling Any Example with CMake in VSCode Specifically
Open VSCode in an example folder, NOT the root folder. From there, so long as the CMakeTools extension is installed, use the build, debug and run buttons along the bottom toolbar to run the program.

## Shader files
Files like *.vert, *.glsl etc. must be translated into header/source files for any given project which uses them. Use Utilities/embed_glsl.c to translate them into their files. Compile and run the executable without any arguments for help.
