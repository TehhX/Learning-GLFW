#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    glfwInit();

    GLFWwindow *window = glfwCreateWindow(600, 800, "MultipleModels", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    gladLoadGL();
    glClearColor(0.2, 0.2, 0.2, 1.0);

    // TODO: Continue

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
}
