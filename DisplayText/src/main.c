// See https://learnopengl.com/In-Practice/Text-Rendering

#include <ft2build.h> // Main/Entry point for freetype2
#include FT_FREETYPE_H // Base ft2 API

#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/vec2.h"

#include "stdio.h"
#include "stdlib.h"

// Source a shader
void gl_shsrc_single(GLuint shader, const char *const src)
{
    glShaderSource(shader, 1, &src, NULL);
}

#define WINDOW_WIDTH  600.0f
#define WINDOW_HEIGHT 800.0f

int main()
{
    // Regular GLFW/GLAD initialization
    glfwInit();

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "DisplayText", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glClearColor(0.2, 0.2, 0.2, 1.0);

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
    if (FT_Set_Pixel_Sizes(face, 0, 48))
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
            texture,
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
        glGenTextures(1, &character_data_arr[current_character].texture);
        glBindTexture(GL_TEXTURE_2D, character_data_arr[current_character].texture);

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
    gl_shsrc_single(vshader, "#version 330 core\n layout (location = 0) in vec4 vertex; out vec2 TexCoords; uniform mat4 projection; void main() { gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); TexCoords = vertex.zw; }");
    gl_shsrc_single(fshader, "#version 330 core\n in vec2 TexCoords; out vec4 color; uniform sampler2D text; uniform vec3 textColor; void main() { vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r); color = vec4(textColor, 1.0) * sampled }");

    // Must enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Projection matrix

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        glClear(GL_COLOR_BUFFER_BIT);

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
