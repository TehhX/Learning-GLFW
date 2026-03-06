#version 330 core

in vec3 vertex_col_to_frag;

void main()
{
    gl_FragColor = vec4(vertex_col_to_frag, 1.0f);
}
