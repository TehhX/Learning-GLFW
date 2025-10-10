# Learning GLFW
A repo to learn the GLFW library, alongside GLAD, OpenGL and CGLM. Made to work crossplatform with Windows/Linux. MSVC is not currently supported, check.

## Repo Structure
The root folder contains the gitignore, README, and license files, along with several folders which contain sub-projects and CMake files within them. All of the folders are their own application, completely seperate from one another, and teach a different thing in GLFW, e.g. EmptyWindow shows the most basic GLFW program, an empty window. It doesn't come with include/ or libs/. Read the below section for more info on third-party libraries etc. `lib-extension` is (usually) `.a` on Linux and `.lib` on windows for static libraries, and `.so` on Linux and `.dll` on Windows for shared/dynamic libraries.

## Installing nesessary files
### Before running any sub-project, make sure you have the following installed:
* GLFW3 - Through a package manager or by placing the relevant files under folder as shown in the below tree
* GLAD - By compiling a static library from their source and then placing the relevant files under this folder as shown in the below tree
* STB_IMAGE - By placing it under this folder as shown in the below tree
* CGLM - Download from [recp/cglm](https://github.com/recp/cglm). I used [ver. 0.9.6](https://github.com/recp/cglm/tree/v0.9.6) in all applicable sub-projects. If any issues arise, they may be caused by using a different version. Place as shown in below tree

### Include/libs tree:
```
+---include
|   |   stb_image.h
|   |
|   +---cglm
|   |       cglm.h
|   |       <rest of .h files and accompanying folders>
|   |
|   +---GLAD
|   |       glad.h
|   |
|   +---GLFW <if not using package manager>
|   |       glfw3.h
|   |       glfw3native.h
|   |
|   \---KHR
|           khrplatform.h
|
+---libs
        libglad.<OS specific lib-extension>
        libglfw.<OS specific lib-extension> <if not using package manager>
        libcglm.<OS specific lib-extension>
```

## Compiling Any Example With CMake
Navigate to a subfolder e.g Shaders, run `embed_glsl.c` if there are any GLSL shader files (more info in [Shader Files](#shader-files)), build with cmake, and run.

## Compiling Any Example with CMake in VSCode Specifically
Add `"cmake.sourceDirectory": "` `<SUB-PROJECT>` `"` to `.vscode/settings.json` where `SUB-PROJECT` is the path to the sub-project you want to compile e.g "/home/`user`/Learning-GLFW/EmptyWindow". From there, so long as the CMakeTools extension is installed, use the build, debug and run buttons along the bottom toolbar to run the program.

## Shader files
Files like `*.vert`, `*.glsl`, `*.frag` etc. must be translated into header/source files for any given sub-project which uses them. Use Utilities/embed_glsl.c to translate them into their files. Compile and run the executable without any arguments for help.

## On The Topic of Windows (OS)
* Windows (graphical element) will freeze when window (graphical element) events are triggered on Windows (OS), stopping *all* code execution on thread-0. This may be able to be solved by keeping all window (graphical element) functionality and GPU-communication on thread-0 where it needs to be, while moving background activity like calculations or translations to another thread
* Windows (OS) requires the subsystem to be specified in CMake or during linking(?) to skip the launch of a new terminal window (graphical element) when launched outside of one
