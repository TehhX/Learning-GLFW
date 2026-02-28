#version 330 core

in vec2 TexCoords;

uniform sampler2D text;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0) * sampled;
}
