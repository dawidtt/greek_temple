#version 330 core

layout(location = 0) in vec4 vertex;

uniform mat4 lightSpaceMatrix; // Macierz transformuj¹ca do przestrzeni œwiat³a
uniform mat4 M;                // Macierz modelu

void main()
{
    gl_Position = lightSpaceMatrix * M * vertex;
}