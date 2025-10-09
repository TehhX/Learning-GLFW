#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 icol;

uniform mat4 transformation;

out vec3 col;

void main()
{
    gl_Position = transformation * vec4(pos, 1, 1);
    col = icol;
}
