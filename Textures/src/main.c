#include "GLAD/glad.h"
#include "GLFW/glfw3.h"

#include "gensh.h"

// For handling imagery
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef GLuint GLref;

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *win = glfwCreateWindow(800, 800, "Textures", NULL, NULL);

    glfwMakeContextCurrent(win);
    gladLoadGL();

    // Set behavior when tex_coords are out of [0,1]. Behavior is per-axis, (x,y,z) = (s,t,r) in the 2nd argument:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    /* If GL_CLAMP_TO_BORDER instead of GL_MIRRORED_REPEAT is desired, a border colour would be required as well. Example:
    GLfloat border_col =
    {
    //  R  G  B  A
        1, 0, 0, 1
    };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_col) */

    /* Setting a texture interpolation option will use the following options:
           * GL_TEXTURE_MIN_FILTER: Sets the behavior when minifying (downscaling) textures
           * GL_TEXTURE_MAG_FILTER: Sets the behabior when magnifying (upscaling) textures
           * GL_NEAREST: The default interpolation method, results in blocky textures when upscaling
           * GL_LINEAR: Bilinear interpolation will take into account more than one surrounding texture pixel (texel) */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set the mipmap to use bilinear interp. when downscaling:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    GLref tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Use stb_image.h to intake image information:
    int tex_width, tex_height, tex_chann_c;
    // WARNING! The path in the next line assumes your terminal is currently inside Textures. The triangle will be black if you aren't.
    unsigned char *tex_data = stbi_load("res/bricks.jpg", &tex_width, &tex_height, &tex_chann_c, 0);

    // Put the texture into the buffer, generate its mipmap (half^n sized images):
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free memory associated with the texture data:
    stbi_image_free(tex_data);

    GLfloat tri[] =
    {
        -1, -1, 0, 0, 0,
        0, 1, 0, .5, 1,
        1, -1, 0, 1, 0,
    };

    GLref vao, vbo;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glCreateBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(tri), tri, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(GLfloat) * 5, (void *) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(GLfloat) * 5, (void *) (sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);

    GLref
        prog = glCreateProgram(),
        svert = glCreateShader(GL_VERTEX_SHADER),
        sfrag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(svert, 1, &tri_vert, NULL);
    glShaderSource(sfrag, 1, &tri_frag, NULL);
    glCompileShader(svert);
    glCompileShader(sfrag);

    glAttachShader(prog, svert);
    glAttachShader(prog, sfrag);
    glLinkProgram(prog);
    glUseProgram(prog);
    glDeleteShader(svert);
    glDeleteShader(sfrag);

    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(win);

    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
