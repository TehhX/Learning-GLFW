#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/call.h"

#include "gensh.h"

typedef GLuint GLref;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *win = glfwCreateWindow(800, 800, "Transformations", 0, 0);
    glfwMakeContextCurrent(win);
    glfwSwapInterval(0);

    gladLoadGL();
    glClearColor(0.2, 0.2, 0.2, 1);

    GLref vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Centered around 0, 0, 1 as calculated on Desmos (https://www.desmos.com/calculator/d9cpyvblms)
    GLfloat tri[] =
    {
    //      X      Y   R   G   B
        -0.57, -0.32,  1,  0,  0,
        +0.00, +0.64,  0,  1,  0,
        +0.57, -0.32,  0,  0,  1
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(tri[0]) * 5, (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(tri[0]) * 5, (void *) (sizeof(tri[0]) * 2));
    glEnableVertexAttribArray(1);

    GLref
        prog = glCreateProgram(),
        vert = glCreateShader(GL_VERTEX_SHADER),
        frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert, 1, &tri_vert, 0);
    glShaderSource(frag, 1, &tri_frag, 0);

    glCompileShader(vert);
    glCompileShader(frag);

    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    glLinkProgram(prog);

    glDeleteShader(vert);
    glDeleteShader(frag);

    glUseProgram(prog);

    // A one time transformation fully demonstrating related topics and usage can be found here in commit 72f7639. It has been changed to a constantly rotating triangle.

    // Get the uniform reference to transformation
    const GLref u_transformation = glGetUniformLocation(prog, "transformation");

    // Used to calculate delta time
    double old_time = glfwGetTime();

    // Main loop
    while (!glfwWindowShouldClose(win))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        static mat4 matrix = GLM_MAT4_IDENTITY_INIT;

        // (new_time - old_time) is the delta in time, and is used to ensure constant speed regardless of loop speed.
        double new_time = glfwGetTime();

        // Rotate matrix around Z axis (facing monitor) by time value around the centroid.
        glmc_rotate(matrix, (new_time - old_time) * 1, (vec3){ 0, 0, 1 });

    #if 0 // Print FPS and matrix each frame
        printf("FPS: %10.2lf | ", 1 / (new_time - old_time));
        glmc_mat4_print(matrix, stdout);
    #endif

        old_time = new_time;

        glUniformMatrix4fv(u_transformation, 1, GL_FALSE, *matrix);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
}
