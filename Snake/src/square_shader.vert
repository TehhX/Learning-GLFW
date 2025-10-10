#version 330 core

layout (location = 0) in vec2 pos;

uniform mat4 trans;

void main()
{
    gl_Position = trans * vec4(pos, 0, 1);
}
