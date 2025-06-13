#version 330 core

// Dane wejściowe
in vec4 fragColor;
in vec2 fragTexCoord;
in vec4 fragNormal;
in vec4 fragVert;
in vec4 fragPosLightSpace;

// Wyjście
out vec4 finalColor;

// Tekstury
uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2DShadow shadowMap;

// NOWA ZMIENNA UNIFORM PRZEKAZANA Z C++
uniform float shadowMapSize;

// Wzór próbkowania Poisson Disk dla gładszych cieni
vec2 poissonDisk[16] = vec2[](
   vec2( -0.94201624, -0.39906216 ), vec2( 0.94558609, -0.76890725 ),
   vec2( -0.094184101, -0.92938870 ), vec2( 0.34495938, 0.29387760 ),
   vec2( -0.91588581, 0.45771432 ), vec2( -0.81544232, -0.87912464 ),
   vec2( -0.38277543, 0.27676845 ), vec2( 0.97484398, 0.75648379 ),
   vec2( 0.44323325, -0.97511554 ), vec2( 0.53742981, -0.47373420 ),
   vec2( -0.26496911, -0.41893023 ), vec2( 0.79197514, 0.19090188 ),
   vec2( -0.24188840, 0.99706507 ), vec2( -0.81409955, 0.91437590 ),
   vec2( 0.19984126, 0.78641367 ), vec2( 0.14383161, -0.14100790 )
);

// Funkcja obliczająca współczynnik cienia
float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float currentDepth = projCoords.z;

    float shadow = 0.0;
    float bias = 0.005;
    // ZMIANA: Używamy uniforma zamiast stałej
    float radius = 1.5 / shadowMapSize;

    for (int i = 0; i < 16; i++) {
        shadow += texture(shadowMap, vec3(projCoords.xy + poissonDisk[i] * radius, currentDepth - bias));
    }
    shadow /= 16.0;

    if(projCoords.z > 1.0) {
        shadow = 1.0;
    }

    return shadow;
}

void main() {
    // --- Oświetlenie ---
    vec3 lightPos = vec3(8.0, 5.0, 10.0);
    vec3 lightColor = vec3(1.0, 1.0, 0.95);
    float ambientStrength = 0.3;

    vec3 N = normalize(fragNormal.xyz);
    vec3 lightDir = normalize(lightPos - fragVert.xyz);

    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 ambient = ambientStrength * lightColor;

    // --- Cienie ---
    float shadow = calculateShadow(fragPosLightSpace);

    // --- Teksturowanie ---
    vec4 texColor = texture(textureMap0, fragTexCoord);

    // Połączenie oświetlenia, cienia i koloru tekstury
    vec3 result = (ambient + shadow * diffuse) * texColor.rgb;

    finalColor = vec4(result, texColor.a);
}