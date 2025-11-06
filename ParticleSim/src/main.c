// #include "GLAD/glad.h"
// #include "GLFW/glfw3.h"
#include "cglm/vec3.h"
#include "cglm/mat4.h"
#include "cglm/affine.h"

#include "stdio.h"

// #define WIN_SX 800
// #define WIN_SY 800

// typedef GLuint GLref;

int main()
{
    mat4 test_mat = GLM_MAT4_IDENTITY_INIT;
    glm_translate(test_mat, (vec3){ 10, 15, 20 });
    printf("(%3.2f, %3.2f, %3.2f)\n", test_mat[3][0], test_mat[3][1], test_mat[3][2]);
    printf("%f\n", glm_vec3_distance((vec3){ 0, 0, 0 }, (vec3){ 3, 4, 0 }));
}

// When doing the thing, add back.
// {
//     if (glfwInit() == GLFW_FALSE)
//     {
//         puts("Could not init.");
//         return 1;
//     }

//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

//     GLFWwindow *main_window = glfwCreateWindow(WIN_SX, WIN_SY, "Particle Simulator", NULL, NULL);
//     if (main_window == NULL)
//     {
//         puts("Could not create GLFWwindow.");
//         return 1;
//     }

//     glfwMakeContextCurrent(main_window);
//     if (glfwGetCurrentContext() != main_window)
//     {
//         puts("Could not make main_window current context.");
//         return 1;
//     }

//     if (gladLoadGL() == GL_FALSE)
//     {
//         puts("Could not load GL lib.");
//         return 1;
//     }

//     glViewport(0, 0, WIN_SX, WIN_SY);
//     glClearColor(0.1, 0.1, 0.1, 1.0);

//     double last_time = glfwGetTime();
//     while (glfwWindowShouldClose(main_window) == GLFW_FALSE)
//     {
//         const double curr_time = glfwGetTime();
//         double delta_ms = curr_time - last_time;
//         last_time = curr_time;

//         glfwPollEvents();
//     }

//     glfwTerminate();
// }
