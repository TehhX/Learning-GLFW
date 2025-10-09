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

    GLfloat tri[] =
    {
    //   X   Y   R   G   B
        -.5, -.5,  1,  0,  0,
         0,  .5,  0,  1,  0,
         .5, -.5,  0,  0,  1
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
    GLref u_transformation = glGetUniformLocation(prog, "transformation");

    // Used to calculate delta time
    double old_time = glfwGetTime();

    // The centroid is the center of the triangle. This simply calculates its X, Y, and Z (should be 1) from "tri". Formula from here: https://www.omnicalculator.com/math/centroid-of-a-triangle
    vec3 tri_centroid =
    {
        (tri[0] + tri[5] + tri[10]) / 3.0f,
        (tri[1] + tri[6] + tri[11]) / 3.0f,
        (1 + 1 + 1) / 3.0f
    };

    // Print the centroid
    printf("Centroid ");
    glmc_vec3_print(tri_centroid, stdout);

    // Main loop
    while (!glfwWindowShouldClose(win))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        static mat4 matrix = GLM_MAT4_IDENTITY_INIT;

        // (new_time - old_time) is the delta in time, and is used to ensure constant speed regardless of loop speed.
        double new_time = glfwGetTime();

        // Rotate matrix around Z axis (facing monitor) by time value around the centroid.
        glmc_rotate_at(matrix, tri_centroid, (new_time - old_time) * 1, (vec3){ 0, 0, 1 });

        old_time = new_time;

        glUniformMatrix4fv(u_transformation, 1, GL_FALSE, *matrix);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
}
