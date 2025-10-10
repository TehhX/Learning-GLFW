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
#include "cglm/call.h"

#include "gensh.h"

#include "limits.h"

#define TICK_LEN_MS 675 // Milliseconds per tick

#define  AREA_SIDE_LEN 40 // Width of play area (squares)

#if AREA_SIDE_LEN * AREA_SIDE_LEN > INT_MAX
    #error Play area too large for snake_len and snake_locations.
#endif

#define   WIN_WIDTH 800.0f // Window width (pixels)
#define  WIN_HEIGHT 800.0f // Window height (pixels)

typedef GLuint GLref; // OpenGL object reference (pointer-like).

// Possible snake directions.
enum snake_dir
{
    sd_up,
    sd_down,
    sd_left,
    sd_right
};

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
    glfwSwapInterval(1);
    gladLoadGL();

    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glClearColor(0, 0, 0, 0);

// GPU setup:
    GLref vbo, ebo, vao;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    // Calculated here: https://www.desmos.com/calculator/zhbydfg1bz.
    #define AREA_OFFSET (2.00f / AREA_SIDE_LEN)

    const GLfloat square_verts[] =
    {
        -              1.00f, -              1.00f, // BL
        -1.00f + AREA_OFFSET, -              1.00f, // BR
        -              1.00f, -1.00f + AREA_OFFSET, // TL
        -1.00f + AREA_OFFSET, -1.00f + AREA_OFFSET, // TR
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(square_verts), square_verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    const GLuint square_indices[] =
    {
        0, 1, 3, // TL
        0, 2, 3, // BR
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

    #define   ABSTOX(ABS) ((ABS) % AREA_SIDE_LEN)
    #define   ABSTOY(ABS) ((ABS) / AREA_SIDE_LEN)
    #define XYTOABS(X, Y) ((Y) * AREA_SIDE_LEN + (X))

    int snake_len = 1;
    int snake_locations[AREA_SIDE_LEN * AREA_SIDE_LEN] = {0};
    snake_locations[0] = XYTOABS(AREA_SIDE_LEN / 2, AREA_SIDE_LEN / 2); // Set starting location

    const GLref utrans = glGetUniformLocation(sprog, "trans");
    enum snake_dir sdir = sd_right;

    double old_time = glfwGetTime();
    // Main loop:
    while (!glfwWindowShouldClose(window))
    {
    // Prep
        glClear(GL_COLOR_BUFFER_BIT);

    // Gameplay
        // TODO: Player input here, outisde of delta condition

        const double new_time = glfwGetTime();
        if (new_time - old_time >= (TICK_LEN_MS / 1000.0f))
        {
            for (int i = snake_len - 1; i >= 1; --i) // Move all parts to their leader
            {
                snake_locations[i] = snake_locations[i - 1];
            }

            snake_locations[0] = XYTOABS( // Move master part in the direction of input
                ABSTOX(snake_locations[0]) + (sdir == sd_right) - (sdir == sd_left),
                ABSTOY(snake_locations[0]) + (sdir == sd_up)    - (sdir == sd_down)
            );

            old_time = new_time;
        }

    // Drawing

        for (int i = 0; i < snake_len; ++i)
        {
            mat4 trans = GLM_MAT4_IDENTITY_INIT;
            glmc_translate(trans, (vec3)
            {
                ABSTOX(snake_locations[i]) * AREA_OFFSET, // X
                ABSTOY(snake_locations[i]) * AREA_OFFSET, // Y
                                                       0, // Z
            });

            glUniformMatrix4fv(utrans, 1, GL_FALSE, *trans);
            glDrawElements(GL_TRIANGLES, sizeof(square_verts) / sizeof(*square_verts), GL_UNSIGNED_INT, 0);
        }

    // Display
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

// Cleanup:
    glfwMakeContextCurrent(0);
    glfwDestroyWindow(window);
    glfwTerminate();
}
