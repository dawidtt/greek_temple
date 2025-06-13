#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6]; // Macierze widoku-projekcji dla ka¿dej z 6 œcian

out vec4 FragPos; // Przeka¿ pozycjê fragmentu do shadera fragmentów

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // Wybierz œciankê cubemapy, do której renderujemy
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}