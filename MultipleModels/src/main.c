#include "gensh.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdio.h"
#include "math.h"

// 2 float vector
typedef float vec2[2];

// Modifiable values
#define TRIANGLE_ORIGIN (vec2){ 0, 0 }
#define TRIANGLE_SIDELEN (float) 1
#define SQUARE_ORIGIN (vec2){ 0, 0 }
#define SQUARE_SIDELEN (float) 1

// Array indices
#define X 0
#define Y 1

void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int viewing_square = 0;

// When 'E' pressed, toggle square to triangle or vice versa
void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        viewing_square = !viewing_square;
        printf("Now viewing the %s.\n", (viewing_square ? "square" : "triangle"));
    }
}

int main()
{
    glfwInit();

    GLFWwindow *window = glfwCreateWindow(600, 800, "MultipleModels", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetKeyCallback(window, key_press_callback);

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

    GLuint vbo_tri, vbo_sqr, vao_tri, ebo, vao_sqr;
    glGenBuffers(1, &vbo_tri);
    glGenBuffers(1, &vbo_sqr);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao_tri);
    glGenVertexArrays(1, &vao_sqr);

    // An equilateral triangle centered about TRIANGLE_ORIGIN with side lengths described by TRIANGLE_SIDELEN
    float triangle_vertices[] =
    {
        /* P0 */
            /* X */ TRIANGLE_ORIGIN[X] - TRIANGLE_SIDELEN / 2.0f,
            /* Y */ TRIANGLE_ORIGIN[Y] - ((TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f),

        /* P1 */
            /* X */ TRIANGLE_ORIGIN[X],
            /* Y */ TRIANGLE_ORIGIN[Y] + ((2.0f * TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f),

        /* P2 */
            /* X */ TRIANGLE_ORIGIN[X] + TRIANGLE_SIDELEN / 2.0f,
            /* Y */ TRIANGLE_ORIGIN[Y] - ((TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f)
    };

    glBindVertexArray(vao_tri);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tri);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    float square_vertices[] =
    {
        /* P0 */
            /* X */ SQUARE_ORIGIN[X] - (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] - (SQUARE_SIDELEN / 2.0f),
        /* P1 */
            /* X */ SQUARE_ORIGIN[X] - (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] + (SQUARE_SIDELEN / 2.0f),
        /* P2 */
            /* X */ SQUARE_ORIGIN[X] + (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] + (SQUARE_SIDELEN / 2.0f),
        /* P3 */
            /* X */ SQUARE_ORIGIN[X] + (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] - (SQUARE_SIDELEN / 2.0f)
    };

    unsigned int square_indices[] =
    {
        /* TL */ 0, 1, 2,
        /* BR */ 2, 3, 0
    };

    glBindVertexArray(vao_sqr);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sqr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo_sqr);
    glDeleteBuffers(1, &vbo_tri);

    // Wireframe mode (optional)
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        if (viewing_square)
        {
            glBindVertexArray(vao_sqr);
            glDrawElements(GL_TRIANGLES, sizeof(square_vertices) / sizeof(*square_vertices), GL_UNSIGNED_INT, NULL);
        }
        else
        {
            glBindVertexArray(vao_tri);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
}
