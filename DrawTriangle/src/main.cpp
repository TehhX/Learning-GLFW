#include <iostream>
#include <memory>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

void drawTriangle(int x, int y) {

}

int main(int argc, char* argv[]) {
    if (!glfwInit())
        return -1;

    // Make windows created after this expression unresizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window { glfwCreateWindow(800, 600, "DrawTriangle", nullptr, nullptr) };
    if (!window)
        return -2;

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        return -3;

    glViewport(0, 0, 800, 600);

    // Vertex buffer stuff

    constexpr float triangleVerts[] {
        0.0f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerts), triangleVerts, GL_STATIC_DRAW);

    constexpr char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0, 0, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
