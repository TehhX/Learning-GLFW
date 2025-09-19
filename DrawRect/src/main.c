#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdlib.h"
#include "stdio.h"

#define WIDTH  1920
#define HEIGHT 1080

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

    GLfloat tri_one[] = {
        -1,  1,  0,
         1, -1,  0,
        -1, -1,  0
    };

    GLfloat tri_two[] = {
        -1,  1,  0,
         1,  1,  0,
         1, -1,  0
    };

    const char vertex_shader_src[] = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1);\n"
        "}\n";
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src, (GLint[]){1});
    glCompileShader(vertex_shader);
    GLint vertex_shader_success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_success);
    if (!vertex_shader_success == GL_FALSE) {
        puts("BAD vertex_shader COMPILE");
        return 1;
    }

    const char fragment_shader_src[] =
        "";

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
        glfwPollEvents();
}
