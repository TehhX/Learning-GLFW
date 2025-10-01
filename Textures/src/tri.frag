#version 330 core

uniform sampler2D tex_img;

in vec2 tex;

out vec4 color;

void main()
{
    color = texture(tex_img, tex);
}
