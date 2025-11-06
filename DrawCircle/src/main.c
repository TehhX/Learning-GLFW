#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdio.h"
#include "math.h"

// VSCode thinks M_PI wasn't defined by math.h (which it was), this is to shut it up.
#ifndef M_PI
    #define M_PI 1234.5678
#endif

#define WIN_SIDE_LEN 800 // Side length of square window
#define BACKGROUND_RGB 0.5, 0.25, 0.1

#define ORIGIN_X -0.2 // Center of the circle (X)
#define ORIGIN_Y 0.2 // Center of the circle (Y)
#define RADIUS 0.1 // How large the circle is in relation to the window [0, 1].

#define CIRCLE_VERT_COUNT 22 // Amount of outer vertices in the circle, one at the origin will always exist in addition.
#define DRAW_TYPE 1 // Either 1 for fill or 0 for wireframe
#define DRAW_INDIVIDUAL -1 // -1 for all, otherwise [0, CIRCLE_VERT_COUNT)

typedef GLuint GLref;

int main()
{
    if (glfwInit() == GLFW_FALSE)
    {
        puts("Could not init.");
        return 1;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow *main_window = glfwCreateWindow(WIN_SIDE_LEN, WIN_SIDE_LEN, "Circle", NULL, NULL);
    if (main_window == NULL)
    {
        puts("Could not create GLFWwindow.");
        return 1;
    }

    glfwMakeContextCurrent(main_window);
    if (glfwGetCurrentContext() != main_window)
    {
        puts("Could not make main_window current context.");
        return 1;
    }

    if (gladLoadGL() == GL_FALSE)
    {
        puts("Could not load GL lib.");
        return 1;
    }

    glViewport(0, 0, WIN_SIDE_LEN, WIN_SIDE_LEN);

    GLref vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    {
        GLfloat circle_verts[(CIRCLE_VERT_COUNT + 1) * 2];
            circle_verts[0] = ORIGIN_X;
            circle_verts[1] = ORIGIN_Y;
        for (int i = 1; i <= CIRCLE_VERT_COUNT; ++i)
        {
            // https://www.desmos.com/calculator/ru8sg90siu
            circle_verts[0 + 2 * i] = cos(2 * M_PI / CIRCLE_VERT_COUNT * i) * RADIUS + ORIGIN_X;
            circle_verts[1 + 2 * i] = sin(2 * M_PI / CIRCLE_VERT_COUNT * i) * RADIUS + ORIGIN_Y;
        }

        glBufferData(GL_ARRAY_BUFFER, sizeof(circle_verts), circle_verts, GL_STATIC_DRAW);
    }

    {
        GLuint circle_indices[3 * CIRCLE_VERT_COUNT];
        for (int i = 0; i < CIRCLE_VERT_COUNT - 1; ++i)
        {
            circle_indices[0 + i * 3] = 0;
            circle_indices[1 + i * 3] = i + 1;
            circle_indices[2 + i * 3] = i + 2;
        }

        circle_indices[0 + (CIRCLE_VERT_COUNT - 1) * 3] = 0;
        circle_indices[1 + (CIRCLE_VERT_COUNT - 1) * 3] = CIRCLE_VERT_COUNT;
        circle_indices[2 + (CIRCLE_VERT_COUNT - 1) * 3] = 1;

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices, GL_STATIC_DRAW);
    }

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    GLref
        shader_prog = glCreateProgram(),
        v_shader = glCreateShader(GL_VERTEX_SHADER),
        f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const char vert_src[] = "#version 330 core\nlayout (location=0) in vec2 layout_locs;void main(){gl_Position=vec4(layout_locs,0,1);}";
    const char frag_src[] = "#version 330 core\nout vec4 col;void main(){col = vec4(0.8, 0.8, 0.8, 1);}";

    glShaderSource(v_shader, 1, &(const char *){ vert_src }, NULL);
    glShaderSource(f_shader, 1, &(const char *){ frag_src }, NULL);

    glCompileShader(v_shader);
    glCompileShader(f_shader);

    glAttachShader(shader_prog, v_shader);
    glAttachShader(shader_prog, f_shader);

    glLinkProgram(shader_prog);

    glDetachShader(shader_prog, v_shader);
    glDetachShader(shader_prog, f_shader);

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    glUseProgram(shader_prog);

    glClearColor(BACKGROUND_RGB, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(DRAW_TYPE ? GL_FRONT : GL_FRONT_AND_BACK, DRAW_TYPE ? GL_FILL : GL_LINE);

    glDrawElements(GL_TRIANGLES, DRAW_INDIVIDUAL >= 0 ? 3 : 3 * CIRCLE_VERT_COUNT, GL_UNSIGNED_INT, (void *) (sizeof(GLuint) * 3 * (DRAW_INDIVIDUAL >= 0 ? DRAW_INDIVIDUAL : 0)));

    glfwSwapBuffers(main_window);

    while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(main_window);
    glfwTerminate();
}
