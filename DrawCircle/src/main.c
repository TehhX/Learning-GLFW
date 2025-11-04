#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdio.h"
#include "math.h"

#define WIN_SIDE_LEN 1000 // Side length of square window
#define ORIGIN_X 0.0 // Center of the circle (X)
#define ORIGIN_Y 0.0 // Center of the circle (Y)
#define RADIUS 0.75 // How large the circle is in relation to the window [0, 1].
#define CIRCLE_VERT_COUNT 32 // Make higher for better looking circle.

typedef GLuint GLref;

int main()
{
    if (sizeof(float) != sizeof(GLfloat))
    {
        puts("Float size mismatch.");
        return 1;
    }

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

    float circle_verts[CIRCLE_VERT_COUNT * 2 + 2] = { ORIGIN_X, ORIGIN_Y };
    for (int i = 0; i < CIRCLE_VERT_COUNT; ++i)
    {
        // https://www.desmos.com/calculator/ru8sg90siu
        circle_verts[0 + 2 * (i + 1)] = cos(2 * M_PI / CIRCLE_VERT_COUNT * i) * RADIUS + ORIGIN_X;
        circle_verts[1 + 2 * (i + 1)] = sin(2 * M_PI / CIRCLE_VERT_COUNT * i) * RADIUS + ORIGIN_Y;
    }

    GLuint circle_indices[3 * CIRCLE_VERT_COUNT + 3] = { 0 };
    for (int i = 0; i < CIRCLE_VERT_COUNT; ++i)
    {
        circle_indices[1 + i * 3] = i + 1;
        circle_indices[2 + i * 3] = i + 2;
    }

    circle_indices[1 + CIRCLE_VERT_COUNT * 3] = CIRCLE_VERT_COUNT;
    circle_indices[2 + CIRCLE_VERT_COUNT * 3] = 1;

    GLref vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_verts), circle_verts, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    GLref shader_prog = glCreateProgram();
    {
        GLref
            v_shader = glCreateShader(GL_VERTEX_SHADER),
            f_shader = glCreateShader(GL_FRAGMENT_SHADER);

        const char vert_src[] = "#version 330 core\nlayout (location=0) in vec2 layout_locs;void main(){gl_Position=vec4(layout_locs,0,1);}";
        const char *const vert_src_ptr = &vert_src[0];

        const char frag_src[] = "#version 330 core\nout vec4 col;void main(){col = vec4(0.8, 0.8, 0.8, 1);}";
        const char *const frag_src_ptr = &frag_src[0];

        glShaderSource(v_shader, 1, &vert_src_ptr, NULL);
        glShaderSource(f_shader, 1, &frag_src_ptr, NULL);

        glCompileShader(v_shader);
        glCompileShader(f_shader);

        glAttachShader(shader_prog, v_shader);
        glAttachShader(shader_prog, f_shader);

        glLinkProgram(shader_prog);

        glDetachShader(shader_prog, v_shader);
        glDetachShader(shader_prog, f_shader);

        glDeleteShader(v_shader);
        glDeleteShader(f_shader);
    }

    glUseProgram(shader_prog);

    glClearColor(0.1, 0.1, 0.1, 1.0);

    // Second argument GL_LINE for wireframe circle, GL_FILL for filled circle.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawElements(GL_TRIANGLES, sizeof(circle_indices) / sizeof(GLuint), GL_UNSIGNED_INT, (void *) 0);

    glfwSwapBuffers(main_window);

    while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(main_window);
    glfwTerminate();
}
