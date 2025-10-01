#version 330 core

uniform sampler2D tex_img;

in vec2 tex;

out vec4 color;

void main()
{
    // Create a color vec4 using texture(sampler2D_tex, texture_coordinates), then isolate the green and alpha channels for fun:
    color = texture(tex_img, tex) * vec4(0, 1, 0, 1);
}
