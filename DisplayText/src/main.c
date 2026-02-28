// See https://learnopengl.com/In-Practice/Text-Rendering

#include <ft2build.h> // Main/Entry point for freetype2
#include FT_FREETYPE_H // Base ft2 API

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/vec2.h"
#include "cglm/vec4.h"
#include "cglm/cam.h"

#include "stdio.h"
#include "stdlib.h"

#include "gensh.h"

// Modifiable values
#define WORDS_SCALE (float) 1
#define WORDS_SIZE (int) 70

#define WORDS_ON_SCREEN "Hello, world!"

#define WORDS_OFFSET_X (float) 30
#define WORDS_OFFSET_Y (float) 300

#define WINDOW_WIDTH  (float) 600
#define WINDOW_HEIGHT (float) 600

// Source a shader
void gl_shsrc_single(GLuint shader, const char *const src)
{
    glShaderSource(shader, 1, &src, NULL);
}

void framebuf_size_callback(__attribute__((unused)) GLFWwindow *window, int new_width, int new_height) {
    // This function changes the OpenGL viewport rectangle posX, posY, sizX, sizY
    glViewport(0, 0, new_width, new_height);
}

// TODO: Text is deformed based on the aspect ratio of the window, e.g taller window = taller text
int main()
{
    // Regular GLFW/GLAD initialization
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DisplayText", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glClearColor(0.2, 0.2, 0.2, 1.0);
    glfwSetFramebufferSizeCallback(window, framebuf_size_callback);

    // Initialize the library, check if error occured
    FT_Library library;
    if (FT_Init_FreeType(&library))
    {
        // If fail, return error
        puts("Failed to initialize FreeType2.");
        return 1;
    }

    // Create a FreeMono face
    FT_Face face;
    if (FT_New_Face(library, "FreeMono.otf", 0, &face))
    {
        // If fail, return error
        puts("Failed to load font face.");
        return 1;
    }

    // Define the size in pixels to extract from the face. Using 0 for width sets the width based on the height dynamically.
    if (FT_Set_Pixel_Sizes(face, 0, WORDS_SIZE))
    {
        puts("Failed to set pixel size.");
        return 1;
    }

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE);

    // Struct to contain data on loaded character
    typedef struct character_data
    {
        unsigned int
            texture_id,
            advance;

        ivec2
            size,
            bearing;
    }
    character_data;

    character_data character_data_arr[128];
    for (unsigned char current_character = 0; current_character < 128; ++current_character)
    {
        // Load specific character from face. FT_LOAD_RENDER tells it to create an 8-bit B&W bitmap.
        if (FT_Load_Char(face, current_character, FT_LOAD_RENDER))
        {
            printf("Failed to load character glyph '%c'.", (char) current_character);
            return 1;
        }

        // Generate a texture for the character
        glGenTextures(1, &character_data_arr[current_character].texture_id);
        glBindTexture(GL_TEXTURE_2D, character_data_arr[current_character].texture_id);

        // Load glyph texture data (More info on each argument via website at top of this file)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D,     GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,     GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,        GL_LINEAR);

        // Store character texture for later use
        memcpy(&character_data_arr[current_character].size,    &(ivec2){ face->glyph->bitmap.width, face->glyph->bitmap.rows }, sizeof(ivec2));
        memcpy(&character_data_arr[current_character].bearing, &(ivec2){ face->glyph->bitmap_left,  face->glyph->bitmap_top },  sizeof(ivec2));
        character_data_arr[current_character].advance = face->glyph->advance.x;
    }

    // Surrender freetype2's resources as they are no longer required
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    GLuint
        program = glCreateProgram(),
        vshader = glCreateShader(GL_VERTEX_SHADER),
        fshader = glCreateShader(GL_FRAGMENT_SHADER);

    // Source shaders
    glShaderSource(vshader, 1, &shader_vert, NULL);
    glShaderSource(fshader, 1, &shader_frag, NULL);

    glCompileShader(vshader);
    glCompileShader(fshader);

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    glUseProgram(program);

    int link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &link_success);

    if (!link_success)
    {
        char info[1024];
        glGetProgramInfoLog(program, 1024, NULL, info);
        puts(info);
        return 1;
    }

    // Must enable blending
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Orthographic perspective
    mat4 ortho_projection;
    glm_ortho(0.0f, WINDOW_HEIGHT, 0.0f, WINDOW_WIDTH, 0.0f, 1.0f, ortho_projection);

    GLint u_projection = glGetUniformLocation(program, "projection");
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, *ortho_projection);

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Play with, figure out.
        float x_offset = WORDS_OFFSET_X;
        glActiveTexture(GL_TEXTURE0);

        for (size_t i = 0; WORDS_ON_SCREEN[i]; ++i)
        {
            const character_data *const current_char_data = character_data_arr + WORDS_ON_SCREEN[i];

            const float
                xpos = x_offset + current_char_data->bearing[0] * WORDS_SCALE,
                ypos = WORDS_OFFSET_Y - (current_char_data->size[1] - current_char_data->bearing[1]) * WORDS_SCALE,

                w = current_char_data->size[0] * WORDS_SCALE,
                h = current_char_data->size[1] * WORDS_SCALE,

                vertices[6][4] =
                {
                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos,     ypos,       0.0f, 1.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },

                    { xpos,     ypos + h,   0.0f, 0.0f },
                    { xpos + w, ypos,       1.0f, 1.0f },
                    { xpos + w, ypos + h,   1.0f, 0.0f }
                }
            ;

            glBindTexture(GL_TEXTURE_2D, current_char_data->texture_id);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x_offset += (current_char_data->advance >> 6) * WORDS_SCALE;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

/*
    Error template:
    ---------------
    if ()
    {
        puts("ERROR");
        return 1;
    }
*/
