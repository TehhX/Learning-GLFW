// Display and graphics:
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

// CGLM:
#include "cglm/vec2.h"
#include "cglm/mat2.h"

// STD:
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "stdbool.h"

// Customizable definitions:
#define WIN_SIDELEN 1000
#define BG_COLOR_RGB 0.1, 0.1, 0.1
#define PART_COUNT 500
#define PARTICLE_RAD_MIN 0.005
#define PARTICLE_RAD_MAX 0.05
#define PART_VERTS 21
#define INIT_VEL_MIN 0.025
#define INIT_VEL_MAX 0.3

// Helper definitions:
typedef GLuint GLref; // Reference to GL component e.g. shader.
typedef GLint GLufrm; // Reference to uniform location.
#define particle_mass(INDEX) (particle_rad[INDEX] * particle_rad[INDEX]) // r^2, pi not required as it is common across all masses and changes no calculations. // TODO: Make individual.
#define X 0 // X index of float[2]'s.
#define Y 1 // Y index of float[2]'s.

// Bypasses requirement for strange syntax.
__attribute__((always_inline)) static inline void gl_shsrc_single(GLref shader, const char *const src)
{
    glShaderSource(shader, 1, &src, NULL);
}

// Returns a random float between min and max.
__attribute__((always_inline)) static inline float frand(float min, float max)
{
    return rand() / (float) RAND_MAX * (max - min) + min;
}

// Returns true or false randomly.
__attribute__((always_inline)) static inline bool brand()
{
    return rand() & 1;
}

int main()
{
    srand(time(NULL));

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
    glClearColor(BG_COLOR_RGB, 1.0);

    vec2
        particle_pos[PART_COUNT],
        particle_vel[PART_COUNT];

    float particle_rad[PART_COUNT];

    // TODO: Make sure the positions (accounting for radii) don't initially overlap with eachother or the walls. Maybe a grid of points based on radius max instead of random.
    for (int i = 0; i < PART_COUNT; ++i)
    {
        particle_rad[i] =    frand(PARTICLE_RAD_MIN, PARTICLE_RAD_MAX);

        particle_pos[i][X] = frand(-1 + particle_rad[i], 1 - particle_rad[i]);
        particle_pos[i][Y] = frand(-1 + particle_rad[i], 1 - particle_rad[i]);

        particle_vel[i][X] = frand(INIT_VEL_MIN, INIT_VEL_MAX) * (brand() ? 1 : -1);
        particle_vel[i][Y] = frand(INIT_VEL_MIN, INIT_VEL_MAX) * (brand() ? 1 : -1);
    }

    vec2 circle_verts[PART_VERTS + 1];

    circle_verts[0][X] = 0;
    circle_verts[0][Y] = 0;

    for (int i = 1; i < PART_VERTS + 1; ++i)
    {
        circle_verts[i][X] = cosf(i * 2 * GLM_PI / PART_VERTS);
        circle_verts[i][Y] = sinf(i * 2 * GLM_PI / PART_VERTS);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circle_indices), circle_indices, GL_STATIC_DRAW);

    GLref
        program = glCreateProgram(),
        vertex = glCreateShader(GL_VERTEX_SHADER),
        fragment = glCreateShader(GL_FRAGMENT_SHADER);

    gl_shsrc_single(vertex, "#version 330 core\nlayout (location = 0) in vec2 circle_verts; uniform vec2 particle_pos; uniform mat2 radius_scaler; void main(){ gl_Position = vec4(radius_scaler * circle_verts + particle_pos, 0, 1); }");
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

    const GLufrm Umat2_radius_scaler = glGetUniformLocation(program, "radius_scaler");
    const GLufrm Uvec2_particle_pos = glGetUniformLocation(program, "particle_pos");

    glPolygonMode(GL_FRONT, GL_FILL);

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

            // Essentially a 2x2 identity matrix scaled by the particle radius.
            const mat2 radius_scaler_matrix = {
                { particle_rad[i], 0.0f },
                { 0.0f, particle_rad[i] }
            };

            glUniformMatrix2fv(Umat2_radius_scaler, 1, GL_FALSE, *radius_scaler_matrix);

            if (particle_pos[i][X] + particle_rad[i] > 1)
            {
                particle_pos[i][X] = 1 - (particle_pos[i][X] - 1) - 2 * particle_rad[i];
                particle_vel[i][X] *= -1;
            }
            else if (particle_pos[i][X] - particle_rad[i] < -1)
            {
                particle_pos[i][X] = -1 + (-1 - particle_pos[i][X]) + 2 * particle_rad[i];
                particle_vel[i][X] *= -1;
            }

            if (particle_pos[i][Y] + particle_rad[i] > 1)
            {
                particle_pos[i][Y] = 1 - (particle_pos[i][Y] - 1) - 2 * particle_rad[i];
                particle_vel[i][Y] *= -1;
            }
            else if (particle_pos[i][Y] - particle_rad[i] < -1)
            {
                particle_pos[i][Y] = -1 + (-1 - particle_pos[i][Y]) + 2 * particle_rad[i];
                particle_vel[i][Y] *= -1;
            }

            // TODO: Remake particle collision system.
            for (int j = 0; j < PART_COUNT; ++j)
            {
                const float distance = glm_vec2_distance(particle_pos[i], particle_pos[j]);
                if (i != j && distance < particle_rad[i] + particle_rad[j])
                {

                }
            }

            glUniform2f(Uvec2_particle_pos, particle_pos[i][X], particle_pos[i][Y]);
            glDrawElements(GL_TRIANGLES, sizeof(circle_indices) / sizeof(unsigned int), GL_UNSIGNED_INT, (void *) 0);
        }

        glfwSwapBuffers(main_window);
        glfwPollEvents();
    }
}
