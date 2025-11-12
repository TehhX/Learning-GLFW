// Display and graphics:
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

// CGLM:
#include "cglm/vec2.h"

// STD:
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"

// Customizable definitions:
#define WIN_SIDELEN 1000
#define PART_COUNT 2000
#define PART_RADIUS 0.005
#define PART_VERTS 8
#define INIT_VEL_MIN -.4
#define INIT_VEL_MAX 0.4

// Helper definitions:
typedef GLuint GLref; // Reference to GL component e.g. shader.
typedef GLint GLufrm; // Reference to uniform location.
#define PART_MASS (PART_RADIUS * PART_RADIUS) // r^2, pi not required as it is common across all masses and changes no calculations
#define X 0 // X index of float[2]'s.
#define Y 1 // Y index of float[2]'s.
#define glShaderSourceSingle(SHADER, CONTENT) glShaderSource(SHADER, 1, &(const char *const){ CONTENT }, NULL)
#define frand(MIN, MAX) (rand() / (float) RAND_MAX * (MAX - MIN) + MIN)

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

    GLFWwindow *const main_window = glfwCreateWindow(WIN_SIDELEN, WIN_SIDELEN, "Particle Simulator", NULL, NULL);
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

    glfwSwapInterval(0);

    glViewport(0, 0, WIN_SIDELEN, WIN_SIDELEN);
    glClearColor(0.1, 0.1, 0.1, 1.0);

    srand(time(NULL));

    vec2 particle_pos[PART_COUNT];
    for (int i = 0; i < PART_COUNT; ++i)
    {
        particle_pos[i][X] = frand(-1 + PART_RADIUS, 1 - PART_RADIUS);
        particle_pos[i][Y] = frand(-1 + PART_RADIUS, 1 - PART_RADIUS);
    }

    vec2 particle_vel[PART_COUNT];
    for (int i = 0; i < PART_COUNT; ++i)
    {
        particle_vel[i][X] = frand(INIT_VEL_MIN, INIT_VEL_MAX);
        particle_vel[i][Y] = frand(INIT_VEL_MIN, INIT_VEL_MAX);
    }

    GLref vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    vec2 circle_verts[PART_VERTS + 1];
        circle_verts[0][X] = 0;
        circle_verts[0][Y] = 0;
    for (int i = 1; i < PART_VERTS + 1; ++i)
    {
        circle_verts[i][X] = PART_RADIUS * cosf(i * 2 * GLM_PI / PART_VERTS);
        circle_verts[i][Y] = PART_RADIUS * sinf(i * 2 * GLM_PI / PART_VERTS);
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_verts), circle_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    GLuint circle_indices[PART_VERTS][3];
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

    glShaderSourceSingle(vertex, "#version 330 core\nlayout (location = 0) in vec2 circle_verts; uniform vec2 particle_pos; void main(){ gl_Position = vec4(circle_verts.x + particle_pos.x, circle_verts.y + particle_pos.y, 0, 1); }");
    glShaderSourceSingle(fragment, "#version 330 core\nvoid main(){ gl_FragColor = vec4(1, 0, 0, 1); }");

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

    const GLufrm ufrm_particle_pos = glGetUniformLocation(program, "particle_pos");

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glfwSetTime(0);
    while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
    {
        const float delta_ms = glfwGetTime();
        glfwSetTime(0);
        printf("FPS: %6.2f\n", 1 / delta_ms);

        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < PART_COUNT; ++i)
        {
            particle_pos[i][X] += particle_vel[i][X] * delta_ms;
            particle_pos[i][Y] += particle_vel[i][Y] * delta_ms;

            // Solve wall collisions:
            if (particle_pos[i][X] + PART_RADIUS > 1)
            {
                particle_pos[i][X] = 1 - (particle_pos[i][X] - 1) - 2 * PART_RADIUS;
                particle_vel[i][X] *= -1;
            }
            else if (particle_pos[i][X] - PART_RADIUS < -1)
            {
                particle_pos[i][X] = -1 + (-1 - particle_pos[i][X]) + 2 * PART_RADIUS;
                particle_vel[i][X] *= -1;
            }

            if (particle_pos[i][Y] + PART_RADIUS > 1)
            {
                particle_pos[i][Y] = 1 - (particle_pos[i][Y] - 1) - 2 * PART_RADIUS;
                particle_vel[i][Y] *= -1;
            }
            else if (particle_pos[i][Y] - PART_RADIUS < -1)
            {
                particle_pos[i][Y] = -1 + (-1 - particle_pos[i][Y]) + 2 * PART_RADIUS;
                particle_vel[i][Y] *= -1;
            }

            for (int j = i + 1; j < PART_COUNT; ++j)
            {
                if (glm_vec2_distance(particle_pos[i], particle_pos[j]) < PART_RADIUS * 2.0)
                {
                    /*
                        TODO: Solve for elastic collision between two particles in two dimensions (Assume all logical statements below are true):
                            * Pt0 == Pt1
                            * Ek0 == Ek1
                            * Glancing blows will impart force more in one axis than the other considering that the particles have radii and are not just points
                    */
                }
            }

            // Change uniform particle pos, draw:
            glUniform2f(ufrm_particle_pos, particle_pos[i][X], particle_pos[i][Y]);
            glDrawElements(GL_TRIANGLES, sizeof(circle_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (void *) 0);
        }

        glfwSwapBuffers(main_window);
        glfwPollEvents();
    }

    glfwTerminate();
}
