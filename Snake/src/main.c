#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/call.h"

#include "gensh.h"

#include "limits.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

#define PRINT_FPS 0 // Checks and prints FPS

#define TICK_LEN_S 0.15f // Seconds per tick

#define  AREA_SIDE_LEN 26 // Width and height of play area (squares)

#if AREA_SIDE_LEN * AREA_SIDE_LEN > INT_MAX
    #error Play area too large for snake_len and snake_locations (Guaranteed integer overflow).
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

/*
TODO: Fix errors:
    * Will loop forever when out of spaces
    * Will theoretically take longer and longer the less spaces there are
    * Potential solution: Go to apple_location = <random>, then loop through values until spot is free. Will have to check entirety of snake every rand, but better. When getting back to <random>, all spaces are filled and program can exit with full complete high score. */
void set_new_apple_location(int *const restrict apple_location, const int *const restrict snake_locations, const int snake_len)
{
    SET_NEW_APPLE_GEN_NEW:
    *apple_location = (rand() / (double) RAND_MAX) * (AREA_SIDE_LEN * AREA_SIDE_LEN);
    {
        for (int i = 0; i < snake_len; ++i)
        {
            if (snake_locations[i] == *apple_location)
            {
                goto SET_NEW_APPLE_GEN_NEW;
            }
        }
    }
}

#if PRINT_FPS
    #include "sys/time.h"
#endif

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
    glfwSwapInterval(!PRINT_FPS);
    gladLoadGL();

    glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);
    glClearColor(0.05, 0.05, 0.15, 1);

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

    const GLref utrans = glGetUniformLocation(sprog, "trans");
    const GLref ucolor_spec = glGetUniformLocation(sprog, "color_spec");

    enum snake_dir
        sdir_actual = sd_right,
        sdir_intended = sd_right;

    #define   ABSTOX(ABS) ((ABS) % AREA_SIDE_LEN)
    #define   ABSTOY(ABS) ((ABS) / AREA_SIDE_LEN)
    #define XYTOABS(X, Y) ((Y) * AREA_SIDE_LEN + (X))

    int snake_len = 1;
    int snake_locations[AREA_SIDE_LEN * AREA_SIDE_LEN] = {0};
    snake_locations[0] = XYTOABS(AREA_SIDE_LEN / 2, AREA_SIDE_LEN / 2); // Set starting location

    srand(time(NULL));
    int apple_location;
    set_new_apple_location(&apple_location, snake_locations, snake_len);

    double goal_time = glfwGetTime() + TICK_LEN_S;

    // Main loop:
    while (!glfwWindowShouldClose(window))
    {
    #if PRINT_FPS
        struct timeval start;
        gettimeofday(&start, NULL);
    #endif
    // Prep
        glClear(GL_COLOR_BUFFER_BIT);

    // Gameplay
        // TODO: Input at lower tick values feels buggy. Hovever, it may be a skill issue.
        if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP)) && (sdir_actual != sd_up && sdir_actual != sd_down)) // W/Up
        {
            sdir_intended = sd_up;
        }
        else if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT)) && (sdir_actual != sd_left && sdir_actual != sd_right)) // A/Left
        {
            sdir_intended = sd_left;
        }
        else if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN)) && (sdir_actual != sd_up && sdir_actual != sd_down)) // S/Down
        {
            sdir_intended = sd_down;
        }
        else if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D) || GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT)) && (sdir_actual != sd_left && sdir_actual != sd_right)) // D/Right
        {
            sdir_intended = sd_right;
        }

        // This block executes once per tick:
        if (glfwGetTime() >= goal_time)
        {
            sdir_actual = sdir_intended;

            // If player hits wall in any of the 4 directions, game over.
            if (((ABSTOY(snake_locations[0]) <=                 0) && ( sd_down == sdir_actual)) || // Hit bottom
                ((ABSTOY(snake_locations[0]) >= AREA_SIDE_LEN - 1) && (   sd_up == sdir_actual)) || // Hit top
                ((ABSTOX(snake_locations[0]) <=                 0) && ( sd_left == sdir_actual)) || // Hit left
                ((ABSTOX(snake_locations[0]) >= AREA_SIDE_LEN - 1) && (sd_right == sdir_actual))  ) // Hit right
            {
                // TODO: Implement game over due to hitting wall.
                puts("Hit wall, game over.");
                exit(0);
            }

            const int new_head_loc = XYTOABS(
                ABSTOX(snake_locations[0]) + (sd_right == sdir_actual) - (sd_left == sdir_actual),
                ABSTOY(snake_locations[0]) + (   sd_up == sdir_actual) - (sd_down == sdir_actual)
            );

            if (new_head_loc == apple_location)
            {
                set_new_apple_location(&apple_location, snake_locations, snake_len++);
            }

            // Check if hitting self: (P.S: Impossible to hit master, master-sub, master-sub-sub or master-sub-sub-sub. Only possible on 4th pos and after (index = 3).)
            for (int i = 3; i < snake_len; ++i)
            {
                if (snake_locations[0] == snake_locations[i])
                {
                    // TODO: Implement game over due to hitting self.
                    puts("Hit self, game over.");
                    exit(0);
                }
            }

            // Move all parts to their leader:
            for (int i = snake_len - 1; i >= 1; --i)
            {
                snake_locations[i] = snake_locations[i - 1];
            }

            snake_locations[0] = new_head_loc;

            if (snake_locations[0] == apple_location)
            {
                set_new_apple_location(&apple_location, snake_locations, snake_len++);
            }

            goal_time += TICK_LEN_S;
        }

    // Drawing
        // Snake
        glUniform1i(ucolor_spec, 1);
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

        // Food
        glUniform1i(ucolor_spec, 0);
        mat4 trans = GLM_MAT4_IDENTITY_INIT;
        glmc_translate(trans, (vec3)
        {
            ABSTOX(apple_location) * AREA_OFFSET, // X
            ABSTOY(apple_location) * AREA_OFFSET, // Y
                                               0, // Z
        });
        glUniformMatrix4fv(utrans, 1, GL_FALSE, *trans);
        glDrawElements(GL_TRIANGLES, sizeof(square_verts) / sizeof(*square_verts), GL_UNSIGNED_INT, 0);

    // Display
        glfwSwapBuffers(window);
        glfwPollEvents();

    #if PRINT_FPS
        struct timeval stop;
        gettimeofday(&stop, NULL);
        printf("FPS: %12.2f\n", 1 / ((stop.tv_usec - start.tv_usec) / ((float) 1e+6)));
    #endif
    }

// Cleanup:
    glfwMakeContextCurrent(0);
    glfwDestroyWindow(window);
    glfwTerminate();
}
