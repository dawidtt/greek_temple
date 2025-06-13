#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // Oblicz odleg�o�� fragmentu od pozycji �wiat�a
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // Znormalizuj odleg�o�� do zakresu [0, 1] u�ywaj�c dalekiej p�aszczyzny
    lightDistance = lightDistance / far_plane;
    
    // Zapisz zlinearyzowan� g��boko��
    gl_FragDepth = lightDistance;
}