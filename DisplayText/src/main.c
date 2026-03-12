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
#define WORDS_SIZE (int) 70

#define WORDS_ON_SCREEN "Hello, world!"

#define WORDS_OFFSET_X (float) 30
#define WORDS_OFFSET_Y (float) 300

#define WINDOW_WIDTH  (float) 600
#define WINDOW_HEIGHT (float) 600

void framebuf_size_callback(__attribute__((unused)) GLFWwindow *window, int new_width, int new_height) {
    // This function changes the OpenGL viewport rectangle posX, posY, sizX, sizY
    glViewport(0, 0, new_width, new_height);
}

// Coordinate indices
#define X 0
#define Y 1

// TODO: Text is deformed based on the aspect ratio of the window, e.g taller window = taller text
int main(int argc, char **argv)
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
    // Will load first argument as font filepath if provided, else just use FreeMono from the repository
    if (FT_New_Face(library, (argc == 2 ? argv[1] : "FreeMono.otf"), 0, &face))
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

    // Disable byte-alignment restriction, as it is not always going to be aligned to 3 bytes as with RGB
    glPixelStorei(GL_UNPACK_ALIGNMENT, GL_TRUE);

    // Struct to contain data of a loaded character texture
    typedef struct character_data
    {
        unsigned int
            texture_id, // GL generated texture ID for binding etc
            advance;    // Advance in 1/64ths of a pixel between this and next character

        ivec2
            size,       // X and Y size of character bounds
            bearing;    // Space between origin and start of character
    }
    character_data;

    character_data character_data_arr[sizeof(WORDS_ON_SCREEN) - 1];
    for (int i = 0; i < sizeof(WORDS_ON_SCREEN) - 1; ++i)
    {
        // Load specific character from face. FT_LOAD_RENDER tells it to create an 8-bit B&W bitmap.
        if (FT_Load_Char(face, WORDS_ON_SCREEN[i], FT_LOAD_RENDER))
        {
            printf("Failed to load character glyph '%c'.", WORDS_ON_SCREEN[i]);
            return 1;
        }

        glGenTextures(1, &character_data_arr[i].texture_id);
        glBindTexture(GL_TEXTURE_2D, character_data_arr[i].texture_id);

        glTexImage2D
        (
            GL_TEXTURE_2D,             // Specifies the target, in this case a 2D texture
            0,                         // Mipmap level, base level is 0
            GL_RED,                    // Specifies the data formatting to use in VRAM. The loaded image is an 8bit B&W mask for the actual color. GL_RED will read 1 byte as all the data for a single pixel which we can use later
            face->glyph->bitmap.width, // The width of the texture
            face->glyph->bitmap.rows,  // The height of the texture
            0,                         // Legacy value, unsure of its purpose. May be set to 0
            GL_RED,                    // Specifies the data formatting of the source image. The source image is in 8bit B&W, so only 1 byte is required per pixel
            GL_UNSIGNED_BYTE,          // Data is stored as bytes, or uchars
            face->glyph->bitmap.buffer // The actual image data to use. freetype2 generated the glyph inside the bitmap buffer under the face->glyph
        );

        glTexParameteri(GL_TEXTURE_2D,     GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,     GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,        GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,        GL_LINEAR);

        memcpy(&character_data_arr[i].size,    &(ivec2){ face->glyph->bitmap.width, face->glyph->bitmap.rows }, sizeof(ivec2));
        memcpy(&character_data_arr[i].bearing, &(ivec2){ face->glyph->bitmap_left,  face->glyph->bitmap_top },  sizeof(ivec2));
        character_data_arr[i].advance = face->glyph->advance.x;
    }

    // Surrender freetype2's resources as they are no longer required
    FT_Done_Face(face);
    FT_Done_FreeType(library);

    GLuint
        program = glCreateProgram(),
        vshader = glCreateShader(GL_VERTEX_SHADER),
        fshader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vshader, 1, &shader_vert, NULL);
    glShaderSource(fshader, 1, &shader_frag, NULL);

    glCompileShader(vshader);
    glCompileShader(fshader);

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    glUseProgram(program);

    GLint link_success;
    glGetProgramiv(program, GL_LINK_STATUS, &link_success);
    if (!link_success)
    {
        char info[1024];
        glGetProgramInfoLog(program, 1024, NULL, info);
        puts(info);
        return 1;
    }

    // Must enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Allocate memory in the buffer. This is required for subsequent BufferSubData calls
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Orthographic perspective. When multiplying by the produced mat4, y values between 0 and WINDOW_HEIGHT will be normalized between -1.0 and +1.0. The other values work similarly
    mat4 ortho_projection;
    glm_ortho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1, ortho_projection);

    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, *ortho_projection);

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        float x_offset = WORDS_OFFSET_X;

        for (size_t i = 0; i < sizeof(WORDS_ON_SCREEN) - 1; ++i)
        {
            const float
                x = x_offset + character_data_arr[i].bearing[X],
                y = WORDS_OFFSET_Y - character_data_arr[i].size[Y] + character_data_arr[i].bearing[Y],

                w = character_data_arr[i].size[X],
                h = character_data_arr[i].size[Y],

                vertices[6][4] =
                {
                    { x,     y + h,   0.0f, 0.0f },
                    { x,     y,       0.0f, 1.0f },
                    { x + w, y,       1.0f, 1.0f },

                    { x,     y + h,   0.0f, 0.0f },
                    { x + w, y,       1.0f, 1.0f },
                    { x + w, y + h,   1.0f, 0.0f }
                }
            ;

            glBindTexture(GL_TEXTURE_2D, character_data_arr[i].texture_id);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x_offset += character_data_arr[i].advance >> 6;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
