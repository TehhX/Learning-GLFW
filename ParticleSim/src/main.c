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
#include "stdbool.h"

// Customizable definitions:
#define WIN_SIDELEN 1000
#define PART_COUNT 2
#define COMMON_PART_RADIUS 0.15 // TODO: Should be a randomized range, customized here and per-particle.
#define PART_VERTS 64
#define INIT_VEL_MIN 1
#define INIT_VEL_MAX 1.5

// Helper definitions:
typedef GLuint GLref; // Reference to GL component e.g. shader.
typedef GLint GLufrm; // Reference to uniform location.
#define PART_RADIUS(PLACEHOLDER) COMMON_PART_RADIUS // TODO: Make individual
#define PART_MASS(PLACEHOLDER) (PART_RADIUS(PLACEHOLDER) * PART_RADIUS(PLACEHOLDER)) // r^2, pi not required as it is common across all masses and changes no calculations. // TODO: Make individual.
#define X 0 // X index of float[2]'s.
#define Y 1 // Y index of float[2]'s.
#define OFFSET(BYTES) ((void *) (BYTES)) // Used for glVertexAttribPointer(...)

// Bypasses requirement for strange syntax.
static inline void gl_shsrc_single(GLref shader, const char src[])
{
    glShaderSource(shader, 1, &src, NULL);
}

bool g_seeded = false;

// Returns a random float between min and max.
static inline float frand(float min, float max)
{
    if (!g_seeded)
    {
        srand(time(NULL));
        g_seeded = true;
    }

    return rand() / (float) RAND_MAX * (max - min) + min;
}

// Returns true or false randomly.
static inline bool brand()
{
    if (!g_seeded)
    {
        srand(time(NULL));
        g_seeded = true;
    }

    return rand() & 1;
}

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

    vec2 circle_verts[PART_VERTS + 1];
        circle_verts[0][X] = 0;
        circle_verts[0][Y] = 0;
    for (int i = 1; i < PART_VERTS + 1; ++i)
    {
        circle_verts[i][X] = PART_RADIUS(i) * cosf(i * 2 * GLM_PI / PART_VERTS);
        circle_verts[i][Y] = PART_RADIUS(i) * sinf(i * 2 * GLM_PI / PART_VERTS);
    }

    // TODO: Make sure they don't initially overlap.
    vec2 particle_pos[PART_COUNT];
    for (int i = 0; i < PART_COUNT; ++i)
    {
        particle_pos[i][X] = frand(-1 + PART_RADIUS(i), 1 - PART_RADIUS(i));
        particle_pos[i][Y] = frand(-1 + PART_RADIUS(i), 1 - PART_RADIUS(i));
    }

    vec2 particle_vel[PART_COUNT];
    for (int i = 0; i < PART_COUNT; ++i)
    {
        particle_vel[i][X] = frand(INIT_VEL_MIN, INIT_VEL_MAX) * (brand() ? 1 : -1);
        particle_vel[i][Y] = frand(INIT_VEL_MIN, INIT_VEL_MAX) * (brand() ? 1 : -1);
    }

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

    GLref vbo, vao, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(circle_verts), circle_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, OFFSET(0));
    glEnableVertexAttribArray(0);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices, GL_STATIC_DRAW);

    GLref
        program = glCreateProgram(),
        vertex = glCreateShader(GL_VERTEX_SHADER),
        fragment = glCreateShader(GL_FRAGMENT_SHADER);

    gl_shsrc_single(vertex, "#version 330 core\nlayout (location = 0) in vec2 circle_verts; uniform vec2 particle_pos; void main(){ gl_Position = vec4(circle_verts.x + particle_pos.x, circle_verts.y + particle_pos.y, 0, 1); }");
    gl_shsrc_single(fragment, "#version 330 core\nvoid main(){ gl_FragColor = vec4(1, 0, 0, 1); }");

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

    // TODO: Move into its own thread for Windows's sake.
    while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
    {
        const float delta_ms = glfwGetTime();
        glfwSetTime(0);
        // printf("FPS: %6.2f\n", 1 / delta_ms);

        glClear(GL_COLOR_BUFFER_BIT);

        for (int i = 0; i < PART_COUNT; ++i)
        {
            particle_pos[i][X] += particle_vel[i][X] * delta_ms;
            particle_pos[i][Y] += particle_vel[i][Y] * delta_ms;

            if (particle_pos[i][X] + PART_RADIUS(i) > 1)
            {
                particle_pos[i][X] = 1 - (particle_pos[i][X] - 1) - 2 * PART_RADIUS(i);
                particle_vel[i][X] *= -1;
            }
            else if (particle_pos[i][X] - PART_RADIUS(i) < -1)
            {
                particle_pos[i][X] = -1 + (-1 - particle_pos[i][X]) + 2 * PART_RADIUS(i);
                particle_vel[i][X] *= -1;
            }

            if (particle_pos[i][Y] + PART_RADIUS(i) > 1)
            {
                particle_pos[i][Y] = 1 - (particle_pos[i][Y] - 1) - 2 * PART_RADIUS(i);
                particle_vel[i][Y] *= -1;
            }
            else if (particle_pos[i][Y] - PART_RADIUS(i) < -1)
            {
                particle_pos[i][Y] = -1 + (-1 - particle_pos[i][Y]) + 2 * PART_RADIUS(i);
                particle_vel[i][Y] *= -1;
            }

            for (int j = 0; j < PART_COUNT; ++j)
            {
                const float distance = glm_vec2_distance(particle_pos[i], particle_pos[j]);
                if (i != j && distance < PART_RADIUS(i) + PART_RADIUS(j))
                {
                    // TODO: Not particularly accurate, fix it with a different algorithm. Tends to break with more than 3 particles, and even then they get stuck together.
                    vec2 direction_vector;

                    glm_vec2_sub(particle_pos[i], particle_pos[j], direction_vector);
                    glm_vec2_normalize(direction_vector);
                    glm_vec2_scale(direction_vector, glm_vec2_norm(particle_vel[i]), particle_vel[i]);
                    glm_vec2_scale(direction_vector, (distance - PART_RADIUS(i) - PART_RADIUS(j)) / 2, direction_vector);
                    glm_vec2_add(direction_vector, particle_pos[i], particle_pos[i]);

                    glm_vec2_sub(particle_pos[j], particle_pos[i], direction_vector);
                    glm_vec2_normalize(direction_vector);
                    glm_vec2_scale(direction_vector, glm_vec2_norm(particle_vel[j]), particle_vel[j]);
                    glm_vec2_scale(direction_vector, (distance - PART_RADIUS(j) - PART_RADIUS(i)) / 2, direction_vector);
                    glm_vec2_add(direction_vector, particle_pos[j], particle_pos[j]);
                }
            }

            glUniform2f(ufrm_particle_pos, particle_pos[i][X], particle_pos[i][Y]);
            glDrawElements(GL_TRIANGLES, sizeof(circle_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, OFFSET(0));
        }

        glfwSwapBuffers(main_window);
        glfwPollEvents();
    }

    glfwTerminate();
}
