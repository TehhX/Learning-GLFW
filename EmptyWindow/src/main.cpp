#include <iostream>

#include <glfw3.h>

int main() {
    // Initializing GLFW:
    if (!glfwInit()) {
        std::cerr << "Could not initialize GLFW.\n";
        return 1;
    }

    // Window details:
    constexpr int width             { 1000 };        // Width of the window.
    constexpr int height            { 1000 };        // Height of the window.
    constexpr char windowName[]     { "My Window" }; // Name of the window.
    GLFWmonitor* const monitor      { nullptr };     // Unsure.
    GLFWwindow*  const prevWindow   { nullptr };     // Unsure.

    // Creating a window and getting its handle:
    GLFWwindow* const window { glfwCreateWindow(width, height, windowName, monitor, prevWindow) };
    if (window == nullptr) {
        std::cerr << "Could not create window.\n";
        return 1;
    }

    // Unsure what this does, has something to do with OpenGL's state maybe:
    glfwMakeContextCurrent(window);

    // Message loop:
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    // Terminate GLFW, free resources:
    glfwTerminate();
}
