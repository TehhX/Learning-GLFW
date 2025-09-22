#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdlib.h"
#include "stdio.h"
#include "sys/time.h"

#define WIDTH  800
#define HEIGHT 800

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

    GLref v_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *v_source =
        "#version 330 core\n"
        "layout (location = 0) in vec3 in_pos;"
        "layout (location = 1) in vec3 aColor;"
        "out vec3 ourColor;"
        "void main() {"
                                // Flipped across the horizontal.
            "gl_Position = vec4(in_pos.x, -in_pos.y, in_pos.z, 1);"
            "ourColor = aColor;"
        "}";
    glShaderSource(v_shader, 1, &v_source, NULL);
    glCompileShader(v_shader);

    GLref f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *f_source =
        "#version 330 core\n"
        "out vec4 FragColor;"
        "in vec3 ourColor;"
        "void main() {"
            "FragColor = vec4(ourColor, 1);"
        "}";
    glShaderSource(f_shader, 1, &f_source, NULL);
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
         1, -1.0, 0, 1, 0, 0
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
        struct timeval start, stop;
        gettimeofday(&start, NULL);

        glClear(GL_COLOR_BUFFER_BIT);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_attribs_1), vertex_attribs_1, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_attribs_2), vertex_attribs_2, GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(win);

        glfwPollEvents();

        gettimeofday(&stop, NULL);
        static unsigned long min = -1UL;
        unsigned long current = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
        if (current < min)
            min = current;

        printf("CURR: %7.lu | MIN: %6.lu | FPS: %6.2f\n", current, min, 1000 / (((float) current) / 1000));
    }

    glDeleteProgram(s_prog);

    glfwTerminate();
}
