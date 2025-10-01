# Learning GLFW
A repo to learn the GLFW library, alongside GLAD and OpenGL. Made to work crossplatform with Windows/Linux.

## Repo Structure
The root folder contains the gitignore, README, and license files, along with several folders that have their own projects and CMake files within them. All of the folders are their own application, completely seperate from one another, and teach a different thing in GLFW, e.g. EmptyWindow shows the most basic GLFW program, an empty window.

## Installing nesessary files
Before trying to compile or run anything, make sure you have GLFW3 installed. It may be installed easily via package manager i.e. apt, or the header and library files may be placed in this project's folder for CMake to access them. GLAD files also need to be installed, and compiled into a "glad.a" library file in /libs/. Also place the GLAD headers inside /include/If you are not using a package manager, you can download the nessessary files from [GLFW's website.](https://www.glfw.org/download.html) glad.c will need to be compiled to glad.a inside /libs/ before compiling any project. After this, all projects will be able to access the header and lib files, so long as your files are placed as such:

<!-- TODO: Update include tree and instructions for stb_image and get_program_path. -->

```
Learning-GLFW
├───include
│   ├───GLAD
│   │       glad.h
│   ├───GLFW
│   │       glfw3.h
│   └───KHR
│          khrplatform.h
└───libs
        glad.a
        libglfw3.a
```

## Compiling Any Example With CMake
Navigate to a subfolder such as EmptyWindow, then create and cd into a build folder (should be empty for now). Run CMake in the command line like: "cmake ..". After this, use a build system e.g. Make, Ninja to compile the files, and then run the resulting executable.

## Compiling Any Example with CMake in VSCode Specifically
Open VSCode in an example folder, NOT the root folder. From there, so long as the CMakeTools extension is installed, use the build, debug and run buttons along the bottom toolbar to run the program.

## Shader files
Files like *.vert, *.glsl etc. must be translated into header/source files for any given project which uses them. Use Utilities/embed_glsl.c to translate them into their files. Compile and run the executable without any arguments for help.
