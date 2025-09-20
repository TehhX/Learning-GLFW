#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdlib.h"
#include "stdio.h"

#define WIDTH  1920
#define HEIGHT 1080

// GLuint's are used as references to data, typedef for clarity.
typedef GLuint GLref;

int main() {
    if (!glfwInit()) {
        puts("BAD INIT");
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Draw Rectangle", NULL, NULL);
    if (!window) {
        puts("BAD CREATE_WINDOW");
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (glfwGetCurrentContext() != window) {
        puts("BAD MAKE_CURRENT_CONEXT");
        return 1;
    }

    if (!gladLoadGL()) {
        puts("BAD LOAD_GL");
        return 1;
    }

    glClearColor(0, 0.25f, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    GLfloat rect[] = {
    /* Clockwise
         X,  Y,  Z, */
        -0.8f, -0.8f,  0,
        -0.8f,  0.8f,  0,
         0.8f,  0.8f,  0,
         0.8f, -0.8f,  0
    };

    GLuint indices[] = {
        0, 1, 3, // Bottom left tri
        1, 2, 3  // Top right tri
    };

    const char *vertex_shader_src =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1);\n"
        "}\n";
    GLref vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
    glCompileShader(vertex_shader);

    GLint vertex_shader_success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_success);
    if (vertex_shader_success == GL_FALSE) {
        puts("BAD vertex_shader COMPILE");
        return 1;
    }

    const char *fragment_shader_src =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main() {\n"
        "    FragColor = vec4(1, 0, 0, 1);\n"
        "}";
    GLref fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
    glCompileShader(fragment_shader);

    GLint fragment_shader_success;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &fragment_shader_success);
    if (fragment_shader_success == GL_FALSE) {
        puts("BAD fragment_shader COMPILE");
        return 1;
    }

    GLref shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    GLint shader_program_success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &shader_program_success);
    if (shader_program_success == GL_FALSE) {
        puts("BAD shader_program LINK");
        return 1;
    }
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);
    glUseProgram(shader_program);

    GLref vertex_arr;
    glGenVertexArrays(1, &vertex_arr);
    glBindVertexArray(vertex_arr);

    GLref vertex_buf;
    glGenBuffers(1, &vertex_buf);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buf);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, NULL);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);

    GLref element_buf;
    glGenBuffers(1, &element_buf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode (Optional)
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
        glfwPollEvents();

    glDeleteVertexArrays(1, &vertex_arr);
    glDeleteBuffers(1, &vertex_buf);
    glDeleteBuffers(1, &element_buf);
    glDeleteProgram(shader_program);

    glfwTerminate();

    return 0;
}
