#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *win = glfwCreateWindow(800, 800, "Transformations", 0, 0);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);

    gladLoadGL();
    glClearColor(0.2, 0.2, 0.2, 1);

    while (!glfwWindowShouldClose(win))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
}
