#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"

#include "gensh.h"

typedef GLuint GLref;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *win = glfwCreateWindow(800, 800, "Transformations", 0, 0);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(6);

    gladLoadGL();
    glClearColor(0.2, 0.2, 0.2, 1);

    GLref vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    GLfloat tri[] =
    {
    //   X   Y   R   G   B
        -1, -1,  1,  0,  0,
         0,  1,  0,  1,  0,
         1, -1,  0,  0,  1
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(tri[0]) * 5, (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(tri[0]) * 5, (void *) (sizeof(tri[0]) * 2));
    glEnableVertexAttribArray(1);

    GLref
        prog = glCreateProgram(),
        vert = glCreateShader(GL_VERTEX_SHADER),
        frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &tri_vert, 0);
    glShaderSource(frag, 1, &tri_frag, 0);

    glCompileShader(vert);
    glCompileShader(frag);

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glLinkProgram(prog);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glUseProgram(prog);

    while (!glfwWindowShouldClose(win))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
}
