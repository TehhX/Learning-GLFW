#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main() {
    // Flipped across the horizontal.
    gl_Position = vec4(in_pos.x, -in_pos.y, in_pos.z, 1);
    ourColor = aColor;
}
