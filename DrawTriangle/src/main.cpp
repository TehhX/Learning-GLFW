#include <iostream>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

void drawTriangle(int x, int y) {

}

int main(int argc, char* argv[]) {
    if (!glfwInit())
        return -1;

    // Make windows created after this expression unresizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window { glfwCreateWindow(800, 600, "Triangle", nullptr, nullptr) };
    if (!window)
        return -2;

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        return -3;

    glViewport(0, 0, 800, 600);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
