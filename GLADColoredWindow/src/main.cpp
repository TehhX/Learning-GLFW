#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

constexpr unsigned int width  { 800 };
constexpr unsigned int height { 600 };

// A function to pass to glfwSetFramebufferSizeCallback when window is resized
void framebufSizeCallback(GLFWwindow*, int newW, int newH) {
    // This function changes the OpenGL viewport rectangle posX, posY, sizX, sizY
    glViewport(0, 0, newW, newH);
}

void processInput(GLFWwindow* window) {
    // Exit window when Escape is pressed:
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void render(GLFWwindow* window) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Sets a clear color
    glClear(GL_COLOR_BUFFER_BIT);         // Clears with the color
    glfwSwapBuffers(window);              // Swaps the fore/background buffers
}

int main() {
    if (!glfwInit())
        return -1;

    GLFWwindow* const window { glfwCreateWindow(width, height, "GLAD Colored Window", nullptr, nullptr) };
    if (!window)
        return -2;

    glfwMakeContextCurrent(window);

    // Sets GLAD up:
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        return -3;

    // Tells GLFW what to do by callback when window is resized by user
    glfwSetFramebufferSizeCallback(window, framebufSizeCallback);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        render(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
