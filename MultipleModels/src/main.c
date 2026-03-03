#include "gensh.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdio.h"
#include "math.h"

// 2 float vector
typedef float vec2[2];

// Modifiable values
#define TRIANGLE_ORIGIN (vec2){ 0.0f, 0.0f }
#define TRIANGLE_SIDELEN (float) 1.0f

// Array indices
#define X 0
#define Y 1

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

    GLuint
        program = glCreateProgram(),
        vshader = glCreateShader(GL_VERTEX_SHADER),
        fshader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vshader, 1, &shader_vert, NULL);
    glShaderSource(fshader, 1, &shader_frag, NULL);
    glCompileShader(vshader);
    glCompileShader(fshader);

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    int program_link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &program_link_success);
    if (!program_link_success)
    {
        char info_log[1024];
        glGetProgramInfoLog(program, 1024, NULL, info_log);
        puts(info_log);
        return 1;
    }

    glUseProgram(program);

    glDetachShader(program, vshader);
    glDetachShader(program, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    GLuint vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // An equilateral triangle centered about TRIANGLE_ORIGIN with side lengths described by TRIANGLE_SIDELEN
    float triangle_vertices[3][2] =
    {
        /* P1 */ {
            /* X */ TRIANGLE_ORIGIN[X] - TRIANGLE_SIDELEN / 2.0f,
            /* Y */ TRIANGLE_ORIGIN[Y] - ((TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f)
        },
        /* P2 */ {
            /* X */ TRIANGLE_ORIGIN[X],
            /* Y */ TRIANGLE_ORIGIN[Y] + ((2.0f * TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f)
        },
        /* P3 */ {
            /* X */ TRIANGLE_ORIGIN[X] + TRIANGLE_SIDELEN / 2.0f,
            /* Y */ TRIANGLE_ORIGIN[Y] - ((TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f)
        }
    };

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
