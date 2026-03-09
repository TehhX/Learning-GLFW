/*
    MultipleModels
        A program to demonstrate multiple buffer objects. Press your number pad keys [0,3] to cycle the different models { 0=Square, 1=Triangle, 2=Hexagon, 3=Pentagon }.

    Model demonstrations:
        * 0=Square && 1=Triangle: Binding different models via their VAO's
        * 2=Hexagon: Seperate VBOs for vertices and colors, that EBO's are also bindable via VAO's via 0=square
        * 3=Pentagon: Using one VBO, but not interleaving data for the sake of glBufferSubData.
*/

#include "gensh.h"

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "stdio.h"
#include "stdlib.h"
#include "math.h"

// 2 float vector
typedef float vec2[2];

// Modifiable values
#define TRIANGLE_ORIGIN (vec2){ 0, -0.25 }
#define TRIANGLE_SIDELEN (float) 2
#define SQUARE_ORIGIN (vec2){ -0.5, 0.5 }
#define SQUARE_SIDELEN (float) 1

// Array indices
#define X 0
#define Y 1

void framebuffer_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int currently_viewing = 0;

// When a given numpad button is released and in range of possible values, view appropriate model
void key_press_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_RELEASE)
    {
        return;
    }

    // Press numpad [0,2] to switch between models
    const int next_viewing = key - GLFW_KEY_KP_0;

    // Is in range of possible model indices
    if (next_viewing >= 0 && next_viewing <= 3)
    {
        currently_viewing = next_viewing;
    }
}

GLuint mm_create_program(const char *const shaderv_text, const char *const shaderf_text)
{
    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &shaderv_text, NULL);
    glCompileShader(vshader);

    int vshader_success;
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &vshader_success);
    if (!vshader_success)
    {
        char info_log[1024];
        glGetShaderInfoLog(vshader, 1024, NULL, info_log);
        puts(info_log);
        abort();
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &shaderf_text, NULL);
    glCompileShader(fshader);

    int fshader_success;
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &fshader_success);
    if (!fshader_success)
    {
        char info_log[1024];
        glGetShaderInfoLog(fshader, 1024, NULL, info_log);
        puts(info_log);
        abort();
    }

    glAttachShader(program, fshader);

    glLinkProgram(program);

    int program_link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &program_link_success);
    if (!program_link_success)
    {
        char info_log[1024];
        glGetProgramInfoLog(program, 1024, NULL, info_log);

        puts(info_log);
        abort();
    }

    glDetachShader(program, vshader);
    glDetachShader(program, fshader);

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return program;
}

// Make linter stop complaining about M_PI being undefined
#ifndef M_PI
    #define M_PI -1
#endif

int main()
{
    // If M_PI was actually undefined, exit. Should be compile-time
    if (M_PI == -1)
    {
        puts("M_PI left undefined, logic no longer correct");
        return 1;
    }

    // Make sure numpad key values are adjacent. Should be compile-time
    if
    (
        GLFW_KEY_KP_0 != GLFW_KEY_KP_1 - 1 ||
        GLFW_KEY_KP_1 != GLFW_KEY_KP_2 - 1 ||
        GLFW_KEY_KP_2 != GLFW_KEY_KP_3 - 1
    )
    {
        puts("KP values not adjacent, logic no longer correct.");
        return 1;
    }

    glfwInit();

    GLFWwindow *window = glfwCreateWindow(600, 800, "MultipleModels", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetKeyCallback(window, key_press_callback);
    glfwSwapInterval(0);

    gladLoadGL();
    glClearColor(0.2, 0.2, 0.2, 1.0);

    GLuint
        trisqr_program = mm_create_program(shader_trisqr_vert, shader_trisqr_frag),
           hex_program = mm_create_program(shader_hex_vert, shader_hex_frag),
          pent_program = mm_create_program(shader_pent_vert, shader_pent_frag)
    ;

    // An equilateral triangle centered about TRIANGLE_ORIGIN with side lengths described by TRIANGLE_SIDELEN
    float triangle_vertices[] =
    {
        /* P0 */
            /* X */ TRIANGLE_ORIGIN[X] - TRIANGLE_SIDELEN / 2.0f,
            /* Y */ TRIANGLE_ORIGIN[Y] - ((TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f),

        /* P1 */
            /* X */ TRIANGLE_ORIGIN[X],
            /* Y */ TRIANGLE_ORIGIN[Y] + ((2.0f * TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f),

        /* P2 */
            /* X */ TRIANGLE_ORIGIN[X] + TRIANGLE_SIDELEN / 2.0f,
            /* Y */ TRIANGLE_ORIGIN[Y] - ((TRIANGLE_SIDELEN * sinf(M_PI / 3.0f)) / 3.0f)
    };

    GLuint vbo_tri;
    glGenBuffers(1, &vbo_tri);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tri);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

    GLuint vao_tri;
    glGenVertexArrays(1, &vao_tri);
    glBindVertexArray(vao_tri);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0); // This is the line which must be preceeded by the binding of a vertex array. This line associates the currently bound VAO with the curr. bound VBO
    glEnableVertexAttribArray(0);

    float square_vertices[] =
    {
        /* P0 */
            /* X */ SQUARE_ORIGIN[X] - (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] - (SQUARE_SIDELEN / 2.0f),
        /* P1 */
            /* X */ SQUARE_ORIGIN[X] - (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] + (SQUARE_SIDELEN / 2.0f),
        /* P2 */
            /* X */ SQUARE_ORIGIN[X] + (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] + (SQUARE_SIDELEN / 2.0f),
        /* P3 */
            /* X */ SQUARE_ORIGIN[X] + (SQUARE_SIDELEN / 2.0f),
            /* Y */ SQUARE_ORIGIN[Y] - (SQUARE_SIDELEN / 2.0f)
    };

    GLuint vbo_sqr;
    glGenBuffers(1, &vbo_sqr);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sqr);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

    GLuint vao_sqr;
    glGenVertexArrays(1, &vao_sqr);
    glBindVertexArray(vao_sqr);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    unsigned int square_indices[] =
    {
        /* TL */ 0, 1, 2,
        /* BR */ 2, 3, 0
    };

    GLuint ebo_sqr;
    glGenBuffers(1, &ebo_sqr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_sqr);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), square_indices, GL_STATIC_DRAW);

    // Pre-calculated via sub-project DrawCircle
    float hex_vertices[] =
    {
        //      X           Y
        0.500000f, -0.500000f,
        0.625000f, -0.283494f,
        0.375000f, -0.283494f,
        0.250000f, -0.500000f,
        0.375000f, -0.716506f,
        0.625000f, -0.716506f,
        0.750000f, -0.500000f
    };

    GLuint vbo_hex_vert;
    glGenBuffers(1, &vbo_hex_vert);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_hex_vert);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hex_vertices), hex_vertices, GL_STATIC_DRAW);

    GLuint vao_hex;
    glGenVertexArrays(1, &vao_hex);
    glBindVertexArray(vao_hex);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);

    // Precalculated via https://github.com/TehhX/Learning-C/blob/main/Assorted/slider_to_rgb.c with 6 steps. Center point set to solid red. To test multiple VBOs single VAO
    float hex_colors[] =
    {
    //          R          G          B
        1.000000f, 0.000000f, 0.000000f,
        1.000000f, 0.498039f, 0.000000f,
        0.796078f, 0.698039f, 0.200000f,
        0.596078f, 0.898039f, 0.400000f,
        0.396078f, 0.898039f, 0.600000f,
        0.196078f, 0.698039f, 0.800000f,
        0.000000f, 0.498039f, 1.000000f
    };

    GLuint vbo_hex_col;
    glGenBuffers(1, &vbo_hex_col);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_hex_col);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hex_colors), hex_colors, GL_STATIC_DRAW);

    // VAO already bound above, no need to rebind
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *) 0);
    glEnableVertexAttribArray(1);

    // Pre-calculated via sub-project DrawCircle
    GLuint hex_indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 6,
        0, 6, 1,
    };

    GLuint ebo_hex;
    glGenBuffers(1, &ebo_hex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_hex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(hex_indices), hex_indices, GL_STATIC_DRAW);

    GLuint vbo_pent;
    glGenBuffers(1, &vbo_pent);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pent);
    // Allocate enough memory for 6 vec2 vertex position and 6 vec3 colors
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * (2 + 3), NULL, GL_DYNAMIC_DRAW);

    GLfloat pent_vertices[] =
    {
        -0.500000, -0.500000,
        -0.345491, -0.024472,
        -0.904508, -0.206107,
        -0.904508, -0.793893,
        -0.345491, -0.975528,
        +0.000000, -0.500000
    };

    // Buffer the data itself
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pent_vertices), pent_vertices);

    GLuint vao_pent;
    glGenVertexArrays(1, &vao_pent);
    glBindVertexArray(vao_pent);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void *) (sizeof(float) * 12));
    glEnableVertexAttribArray(1);

    GLuint pent_indices[] =
    {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 5,
        0, 5, 1
    };

    GLuint ebo_pent;
    glGenBuffers(1, &ebo_pent);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_pent);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pent_indices), pent_indices, GL_STATIC_DRAW);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        switch (currently_viewing)
        {
        // Square
        case 0:
            glUseProgram(trisqr_program);
            glBindVertexArray(vao_sqr);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
            break;

        // Triangle
        case 1:
            glUseProgram(trisqr_program);
            glBindVertexArray(vao_tri);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;

        // Hexagon
        case 2:
            glUseProgram(hex_program);
            glBindVertexArray(vao_hex);
            glDrawElements(GL_TRIANGLES, sizeof(hex_indices) / sizeof(*hex_indices), GL_UNSIGNED_INT, NULL);
            break;

        // Pentagon
        case 3:
            glUseProgram(pent_program);
            glBindVertexArray(vao_pent);

            // Change colors based on time, put into the same buffer as that of the vertices
            float pent_colors[6 * 3];
            for (int i = 0; i < 6 * 3; i += 3)
            {
                pent_colors[i]     = +sinf(glfwGetTime()) / 2.0f + 1.0f;
                pent_colors[i + 1] = +cosf(glfwGetTime()) / 2.0f + 1.0f;
                pent_colors[i + 2] = -sinf(glfwGetTime()) / 2.0f + 1.0f;
            }
            glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 2, sizeof(pent_colors), pent_colors);

            glDrawElements(GL_TRIANGLES, sizeof(pent_indices) / sizeof(*pent_indices), GL_UNSIGNED_INT, NULL);
            break;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &vbo_sqr);
    glDeleteVertexArrays(1, &vao_sqr);
    glDeleteBuffers(1, &ebo_sqr);

    glDeleteBuffers(1, &vbo_tri);
    glDeleteVertexArrays(1, &vao_tri);

    glDeleteBuffers(1, &vbo_hex_vert);
    glDeleteBuffers(1, &vbo_hex_col);
    glDeleteVertexArrays(1, &vao_hex);

    glDeleteBuffers(1, &vbo_pent);
    glDeleteVertexArrays(1, &vao_pent);

    glDeleteProgram(trisqr_program);
    glDeleteProgram(hex_program);
    glDeleteProgram(pent_program);

    glfwMakeContextCurrent(NULL);
    glfwDestroyWindow(window);
    glfwTerminate();
}
