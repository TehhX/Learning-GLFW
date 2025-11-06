// Display and graphics:
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

// STD:
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

// Customizable definitions:
#define WIN_SX 800
#define WIN_SY 800
#define PART_COUNT 1
#define PART_RADIUS 0.025
#define PART_VERTS 11
#define INIT_SPD_MIN 0.0
#define INIT_SPD_MAX 1.0

// Helper definitions:
typedef GLuint GLref;
#define X 0
#define Y 1
#define SHAD_SRC(CONTENT) (&(const char *const){ (const char[]){ CONTENT } })

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

    GLFWwindow *main_window = glfwCreateWindow(WIN_SX, WIN_SY, "Particle Simulator", NULL, NULL);
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

    glViewport(0, 0, WIN_SX, WIN_SY);
    glClearColor(0.1, 0.1, 0.1, 1.0);

    float particle_pos[PART_COUNT][2];
    for (int i = 0; i < PART_COUNT; ++i)
    {
        // TODO: Randomize starting positions in range [-1 + radius, 1 - radius].
        particle_pos[i][X] = 0;
        particle_pos[i][Y] = 0;
    }

    float particle_vel[PART_COUNT][2];
    for (int i = 0; i < PART_COUNT; ++i)
    {
        // TODO: Randomize velocities, between very slow and cross-screen in a few hundred ms.
        particle_vel[i][X] = 0.12;
        particle_vel[i][Y] = 0.2;
    }

    GLref vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    float circle_verts[PART_VERTS + 1][2];
    circle_verts[0][X] = 0;
    circle_verts[0][Y] = 0;
    for (int i = 1; i < PART_VERTS + 1; ++i)
    {
        circle_verts[i][X] = PART_RADIUS * cosf(i * 2 * M_PI / PART_VERTS);
        circle_verts[i][Y] = PART_RADIUS * sinf(i * 2 * M_PI / PART_VERTS);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_verts), circle_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    unsigned int circle_indices[PART_VERTS][3];
    for (int i = 0; i < PART_VERTS - 1; ++i)
    {
        circle_indices[i][0] = 0;
        circle_indices[i][1] = i + 1;
        circle_indices[i][2] = i + 2;
    }
    circle_indices[PART_VERTS - 1][0] = 0;
    circle_indices[PART_VERTS - 1][1] = PART_VERTS;
    circle_indices[PART_VERTS - 1][2] = 1;

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices, GL_STATIC_DRAW);

    GLref
        program = glCreateProgram(),
        vertex = glCreateShader(GL_VERTEX_SHADER),
        fragment = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex, 1, SHAD_SRC("#version 330 core\nlayout (location = 0) in vec2 circle_verts; uniform vec2 particle_pos; void main(){ gl_Position = vec4(circle_verts.x + particle_pos.x, circle_verts.y + particle_pos.y, 0, 1); }"), NULL);
    glShaderSource(fragment, 1, SHAD_SRC("#version 330 core\nout vec4 col; void main(){ col = vec4(1, 0, 0, 1); }"), NULL);

    glCompileShader(vertex);
    glCompileShader(fragment);

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);
    glUseProgram(program);

    glDetachShader(program, vertex);
    glDetachShader(program, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint ufrm_particle_pos = glGetUniformLocation(program, "particle_pos");

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float last_time = glfwGetTime();
    while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
    {
        const float curr_time = glfwGetTime();
        float delta_ms = curr_time - last_time;
        last_time = curr_time;

        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < PART_COUNT; ++i)
        {
            particle_pos[i][X] += particle_vel[i][X] * delta_ms;
            particle_pos[i][Y] += particle_vel[i][Y] * delta_ms;

            glUniform2f(ufrm_particle_pos, particle_pos[i][X], particle_pos[i][Y]);
            glDrawElements(GL_TRIANGLES, sizeof(circle_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (void *) 0);
        }

        glfwSwapBuffers(main_window);
        glfwPollEvents();
    }

    glfwTerminate();
}
