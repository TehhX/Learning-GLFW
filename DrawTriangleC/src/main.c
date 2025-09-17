#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#define     WIDTH 1280
#define    HEIGHT  720
#define INFO_SIZE  512

int main(int argc, char **argv) {
    // Initializes GLFW. Required before anything else.
    if (!glfwInit())
        return 1;

    // Make windows created after this expression unresizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window and assign window its handle.
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "DrawTriangle", NULL, NULL);
    if (!window)
        return 1;

    // Makes window the current GL context. OpenGL requires a "context" for its functionality.
    glfwMakeContextCurrent(window);

    // Demonstrates the last line's change. As window *is* the current context, glfwGetCurrentContext == window.
    if (glfwGetCurrentContext() != window)
        return 1;

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        return 1;

    glViewport(0, 0, WIDTH, HEIGHT);

    // Vertex buffer stuff
    const float triangleVerts[] = {
        -0.5f, -0.7f, 0.0f,
         0.5f, -0.7f, 0.0f,
         0.0f,  0.7f, 0.0f
    };

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerts), triangleVerts, GL_STATIC_DRAW);

    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[INFO_SIZE];
        glGetShaderInfoLog(vertexShader, INFO_SIZE, NULL, infoLog);
        puts(infoLog);
        return 1;
    }

    const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main() {\n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}";

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[INFO_SIZE];
        glGetShaderInfoLog(shaderProgram, INFO_SIZE, NULL, infoLog);
        puts(infoLog);
        return 1;
    }

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerts), triangleVerts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
}
