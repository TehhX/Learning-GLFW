#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdio.h"

#define WIN_SX 800
#define WIN_SY 800

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

    float particle_pos[] =
    {
    //  Units from center
    //   PX   PY
        0.0, 0.0
    };

    float particle_vel[] =
    {
    //  Units/second
    //   DX   DY
        1.0, 1.0
    };

    if (sizeof(particle_pos) != sizeof(particle_vel))
    {
        puts("Particle array size mismatch.");
        return 1;
    }

    const int particle_count = sizeof(particle_pos) / sizeof(float) / 2;

    double last_time = glfwGetTime();
    while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
    {
        const double curr_time = glfwGetTime();
        double delta_ms = curr_time - last_time;
        last_time = curr_time;

        for (int i = 0; i < particle_count; ++i)
        {
            particle_pos[0 + i * 2] += particle_vel[0 + i * 2] * delta_ms;
            particle_pos[1 + i * 2] += particle_vel[1 + i * 2] * delta_ms;

            printf
            (
                "Particle #%2d:\n\tPOS(%0.2f, %0.2f)\n\tVEL(%0.2f, %0.2f)\n",
                i + 1,                   // Index
                particle_pos[0 + i * 2], // PX
                particle_pos[1 + i * 2], // PY
                particle_vel[0 + i * 2], // DX
                particle_vel[1 + i * 2]  // DY
            );
        }

        glfwPollEvents();
    }

    glfwTerminate();
}
