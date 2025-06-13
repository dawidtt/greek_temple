#version 330 core

layout (location = 0) in vec4 vertex;

uniform mat4 M;

void main()
{
    gl_Position = M * vertex;
}