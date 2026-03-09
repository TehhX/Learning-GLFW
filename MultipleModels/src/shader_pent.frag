#version 330 core

in vec3 color_to_frag;

void main()
{
    gl_FragColor = vec4(color_to_frag, 1.0);
}
