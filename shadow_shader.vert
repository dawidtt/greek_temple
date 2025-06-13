#version 330 core

layout(location = 0) in vec4 vertex;

uniform mat4 lightSpaceMatrix; // Macierz transformuj�ca do przestrzeni �wiat�a
uniform mat4 M;                // Macierz modelu

void main()
{
    gl_Position = lightSpaceMatrix * M * vertex;
}