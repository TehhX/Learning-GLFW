#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

// Make sure to translate shaders.vert and shaders.frag into header/source files. More info in ROOT/README.md/Shader Files.
#include "generated_shaders.h"

#include "stdlib.h"
#include "stdio.h"

// Define to calculate FPS each frame, else comment out. Requires "sys/time.h".
#define CALCULATE_FPS

#ifdef CALCULATE_FPS
#include "sys/time.h"
#endif

#define WIDTH  800
#define HEIGHT 800

// Microseconds to seconds conversion:
#define ustos(US) ((US) / ((float) 1e+6))

// A reference to a GL object is also an unsigned integer. Typedef for clairty.
typedef GLuint GLref;

int main() {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *win = glfwCreateWindow(WIDTH, HEIGHT, "Shaders", NULL, NULL);

    glfwMakeContextCurrent(win);

    gladLoadGL();

    // Don't limit FPS. Default is 1 for vsync (seems to limit to monitor hz e.g fps <= 150 on 144hz monitor), 0 seems to uncap.
    glfwSwapInterval(0);

    GLref v_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader, 1, &shaders_vert, NULL);
    glCompileShader(v_shader);

    GLref f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader, 1, &shaders_frag, NULL);
    glCompileShader(f_shader);

    GLref s_prog = glCreateProgram();
    glAttachShader(s_prog, v_shader);
    glAttachShader(s_prog, f_shader);
    glLinkProgram(s_prog);
    glUseProgram(s_prog);
    glDeleteShader(v_shader);
    glDeleteShader(f_shader);

    GLfloat vertex_attribs_1[] = {
    //          POS                 COL
    //     X      Y    Z    R    G    B
        -0.6, -0.45,   0,   1,   0,   0,
         0.6, -0.45,   0,   0,   1,   0,
         0.0,  0.45,   0,   0,   0,   1
    };

    GLfloat vertex_attribs_2[] = {
        -1, -1.0, 0, 0, 0, 1,
         0, -0.1, 0, 0, 1, 0,
         1, -1.0, 0, 0, 1, 0
    };

    GLref v_arr;
    glGenVertexArrays(1, &v_arr);
    glBindVertexArray(v_arr);

    GLref v_buf;
    glGenBuffers(1, &v_buf);
    glBindBuffer(GL_ARRAY_BUFFER, v_buf);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void *) (sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    glClearColor(0.3, 0.3, 0.3, 1);

    while (!glfwWindowShouldClose(win)) {
#ifdef CALCULATE_FPS
        struct timeval start;
        gettimeofday(&start, NULL);
#endif
        glClear(GL_COLOR_BUFFER_BIT);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_attribs_1), vertex_attribs_1, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_attribs_2), vertex_attribs_2, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);

        glfwPollEvents();
#ifdef CALCULATE_FPS
        struct timeval stop;
        gettimeofday(&stop, NULL);
        printf("FPS: %12.2f\n", 1 / ustos(stop.tv_usec - start.tv_usec));
#endif
    }

    glDeleteProgram(s_prog);

    glfwTerminate();
}
