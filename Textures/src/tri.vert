#version 330 core

layout (location = 0) in vec3 ipos;
layout (location = 1) in vec2 itex;

out vec2 tex;

void main()
{
    // Testing single-line comments
    gl_Position = vec4(ipos, 1);/*
        Testing multi-line comments.
        They work as such.
            And so forth. // Single-line comments inside multi-line comments.
    *//* Another comment! */
    tex = itex;
}
