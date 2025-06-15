#version 330 core

// wejście
layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec2 texCoord0;

// wyjście
out vec4 fragColor;
out vec2 fragTexCoord;
out vec4 fragNormal;
out vec4 fragVert;
out vec4 fragPosLightSpace; 

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 lightSpaceMatrix; 

void main() {
    gl_Position = P * V * M * vertex;
    fragColor = color;
    fragTexCoord = texCoord0;
    fragNormal = M * normal;
    fragVert = M * vertex;
    fragPosLightSpace = lightSpaceMatrix * M * vertex; 
}