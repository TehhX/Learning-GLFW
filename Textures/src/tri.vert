#version 330 core

layout (location = 0) in vec3 ipos;
layout (location = 1) in vec2 itex;

out vec2 tex;

void main()
{
    gl_Position = vec4(ipos, 1);
    tex = itex;
}
