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

    // Specify GLFW version and profile. This was made in version 3.3, and only uses modern functionality. Because of this, compatability is *not* required, and core is used.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Make windows created after this expression unresizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window and assign window its handle. The fourth argument (monitor) specifies the monitor if exclusive fullscreen is desired, NULL if windowed. The last argument (share) is for sharing resources between windows, and is not required.
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

    // Screen location of each of the triangle's vertices. The screen starts at (-1, -1, 0) at the top left, and ends at (1, 1, 0) bottom right.
    GLfloat triangle_vertices[] = {
        -0.5f, -0.7f, 0.0f,
         0.5f, -0.7f, 0.0f,
         0.0f,  0.7f, 0.0f
    };

    // Vertex shader, creates the locations of the vertices.
    const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader, creates the colour of the vertices.
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

    // Shader program 
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Deletes the shaders used in creating the program, as they have been used and are not required anymore.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Makes sure the shader program was a success.
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[INFO_SIZE];
        glGetShaderInfoLog(vertexShader, INFO_SIZE, NULL, infoLog);
        puts(infoLog);
        return 1;
    }

    GLuint vertex_buffer_object, // Is used to transmit a large buffer of data to the GPU at once.
           vertex_array_object;  // Stores pointers to (n >= 1) vertex_buffer_object's and tells OpenGL how to use them.

    glGenVertexArrays(1, &vertex_array_object);
    glGenBuffers(1, &vertex_buffer_object);

    glBindVertexArray(vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Sets background colour.
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(vertex_array_object);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Buffers are switched between frames so as to not flicker the screen by drawing data directly onto the screen. The above lines are applied to the unused buffer. This swaps the unused buffer with the used one.
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window))
        glfwPollEvents();

    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteBuffers(1, &vertex_buffer_object);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
}
