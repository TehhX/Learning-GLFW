#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

#define     WIDTH 1280
#define    HEIGHT  720
#define INFO_SIZE  512

int main() {
    // Initializes GLFW. Required before anything else.
    if (!glfwInit())
        return 1;

    // Specify GLFW version and profile. This was made in version 3.3, and only uses modern functionality. Because of this, compatability is *not* required, and core is used.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Make windows created after this expression unresizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Turns on the window border which contains minimize, maximize, close. On be default. Can also be turned off.
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);

    // Create a window and assign window its handle. The fourth argument (monitor) specifies the monitor if exclusive fullscreen is desired, NULL if windowed. The last argument (share) is for sharing resources between windows, and is not required.
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "DrawTriangle", NULL, NULL);
    if (!window)
        return 1;

    // Makes window the current GL context. OpenGL requires a "context" for its functionality.
    glfwMakeContextCurrent(window);

    // Demonstrates the last line's change. As window *is* the current context, glfwGetCurrentContext == window.
    if (glfwGetCurrentContext() != window)
        return 1;

    // Loads GL functions into the current context. Alternative to gladLoadGLLoader(...) from earlier commits.
    if (!gladLoadGL())
        return 1;

    // The first two arguments specify offset from the bottom left of the window (x, y). The last two the width and height (w, h). IMPORTANT NOTE: It is NOT from top left as with most graphical programs. Not calling it seems to make it fit the window.
    glViewport(0, 0, WIDTH, HEIGHT);

    // Will take screen co-ordinates in pixels like ([0, WIDTH], [0, HEIGHT]) and put them into the context of [-1, 1]. Use ...GL_X for X co-ord, ...GL_Y for Y co-ord.
    #define __STD_TO_GL(VAL, MAX) ((VAL) * (2.0f / (MAX)) - 1)
    #define STD_TO_GL_X(VAL) (__STD_TO_GL((VAL), WIDTH))
    #define STD_TO_GL_Y(VAL) (__STD_TO_GL((VAL), HEIGHT) * -1)

    // General triangle sizer (Pixels).
    #define TRI_SIZE 250

    // Screen location of each of the triangle's vertices. The screen starts at (-1, -1, -1) at the back top left, and ends at (1, 1, 1) front bottom right.
    GLfloat triangle_vertices[] = {
        STD_TO_GL_X(640 - 1.5f * TRI_SIZE), STD_TO_GL_Y(360 + TRI_SIZE), 0,
        STD_TO_GL_X(640 + 1.5f * TRI_SIZE), STD_TO_GL_Y(360 + TRI_SIZE), 0,
        STD_TO_GL_X(640),                   STD_TO_GL_Y(360 - TRI_SIZE), 0
    };

    // Vertex shader, creates the position of the verticies. GLuint's are references to GL data e.g a shader, similar to pointers (I think).
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    static const char *vertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() {\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}";
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader, creates the colour of the vertices.
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    static const char *fragmentShaderSource =
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "void main() {\n"
        "    FragColor = vec4(0.25f, 0.8f, 0.6f, 1);\n"
        "}";
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // The shader program will hold all shaders created to make the triangle.
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

    // Cleanup.
    glDeleteVertexArrays(1, &vertex_array_object);
    glDeleteBuffers(1, &vertex_buffer_object);
    glDeleteProgram(shaderProgram);

    // Terminate GLFW as it is no longer required.
    glfwTerminate();
}
