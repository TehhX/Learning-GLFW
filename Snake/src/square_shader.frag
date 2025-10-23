#version 330 core

out vec4 color;

uniform bool color_spec;

void main()
{
    color = vec4(!color_spec, color_spec, 0, 1);
}
