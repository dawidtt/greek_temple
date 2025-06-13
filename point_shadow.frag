#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // Oblicz odleg³oœæ fragmentu od pozycji œwiat³a
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // Znormalizuj odleg³oœæ do zakresu [0, 1] u¿ywaj¹c dalekiej p³aszczyzny
    lightDistance = lightDistance / far_plane;
    
    // Zapisz zlinearyzowan¹ g³êbokoœæ
    gl_FragDepth = lightDistance;
}