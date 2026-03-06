#version 330 core

layout (location = 0) in vec2 vertex_pos;
layout (location = 1) in vec3 vertex_col;

out vec3 vertex_col_to_frag;

void main()
{
    gl_Position = vec4(vertex_pos, 0.0, 1.0);
    vertex_col_to_frag = vertex_col;
}
