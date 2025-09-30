#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

typedef GLuint GLref;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *win = glfwCreateWindow(800, 800, "Textures", NULL, NULL);

    glfwMakeContextCurrent(win);
    gladLoadGL();
    glfwSwapInterval(0);

    GLref vao, vbo;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLfloat tri[] =
    {
        -1, -1, 0,
        0, 1, 0,
        1, -1, 0,
    };

    // TODO: Continue.

    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(win);
    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
