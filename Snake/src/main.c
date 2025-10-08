/*
    Snake Brainstorming:
        * Snake will require (data):
            * A current heading of the snake
            * Current length of the snake
            * Some way of representing the snake parts which comprise it (array etc.)
            * Current score
            * Past high scores
            * Width/height
            * Time
        * Snake will need to (function):
            * Take user input of esc to quit, wasd/arr-keys for movement, space to restart
            * Display a black background, deco, no resizing
            * Read/write high scores to disk
            * Display on top of that:
                * Main menu:
                    * Controls
                    * Maybe a randomly controlled snake graphic
                * Gameplay:
                    * Current score
                    * Snake
                    * Food
                    * Borders
                * Game over:
                    * Current score, past high scores
            * Calculate if the snake hits itself/wall
            * If input would go against itself, ignore
            * Speed-increase
*/

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "gensh.h"

#include "stdio.h"

#define TICK_LEN_MS 800 // Milliseconds per tick

#define   WIN_WIDTH 800 // Window width (pixels)
#define  WIN_HEIGHT 800 // Window height (pixels)

#define  AREA_WIDTH  40 // Width of play area (squares)
#define AREA_HEIGHT  40 // Height of play area (squares)

typedef GLuint GLref; // OpenGL object reference (pointer-like).

// Possible snake directions.
typedef enum
{
    up,
    down,
    left,
    right
}
snake_dir;

int main()
{
// Initialization:
    glfwInit();

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Snake", 0, 0);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glClearColor(0, 0, 0, 0);

// GPU setup:
    GLref vbo, ebo, vao;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    const GLfloat square_verts[] =
    {
        -1, -1, 0,
        -1, 1, 0,
        1, 1, 0,
        1, -1, 0
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(square_verts), square_verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void *) 0);
    glEnableVertexAttribArray(0);

    const GLuint square_indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

// Shader program setup:
    GLref
        sprog = glCreateProgram(),
        svert = glCreateShader(GL_VERTEX_SHADER),
        sfrag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(svert, 1, &square_shader_vert, 0);
    glShaderSource(sfrag, 1, &square_shader_frag, 0);

    glCompileShader(svert);
    glCompileShader(sfrag);

    glAttachShader(sprog, svert);
    glAttachShader(sprog, sfrag);
    glLinkProgram(sprog);

    glDeleteShader(svert);
    glDeleteShader(sfrag);

    glUseProgram(sprog);

// Main loop:
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, sizeof(square_indices) / sizeof(square_indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

// Cleanup:
    glfwMakeContextCurrent(0);
    glfwDestroyWindow(window);
    glfwTerminate();
}
