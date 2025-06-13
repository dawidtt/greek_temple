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
uniform sampler2DShadow shadowMap;
uniform samplerCube shadowCubeMap;

// Uniformy
uniform float shadowMapSize;
uniform vec3 viewPos;
uniform float far_plane_point;
uniform vec3 directionalLightPos;
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float linear;
uniform float quadratic;

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

// Funkcje pomocnicze
float calculateDirShadow(vec4);
float calculatePointShadow(vec3);

void main() {
    // Wspólne zmienne
    vec3 N = normalize(fragNormal.xyz);
    vec3 viewDir = normalize(viewPos - fragVert.xyz);
    vec4 texColor = texture(textureMap0, fragTexCoord);
    float shininess = 32.0;

    // słońce
    vec3 directionalLightColor = vec3(1.0, 1.0, 0.95);
    vec3 ambient = 0.3 * directionalLightColor;
    vec3 lightDir = normalize(directionalLightPos - fragVert.xyz);
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = diff * directionalLightColor;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(N, halfwayDir), 0.0), shininess);
    vec3 specular = spec * directionalLightColor;
    float shadow = calculateDirShadow(fragPosLightSpace);
    vec3 directionalResult = (diffuse + specular) * shadow;

    // Światło punktowe
    vec3 pointLightDir = normalize(pointLightPos - fragVert.xyz);
    float pointDiff = max(dot(N, pointLightDir), 0.0);
    vec3 pointDiffuse = pointDiff * pointLightColor;
    vec3 pointHalfwayDir = normalize(pointLightDir + viewDir);
    float pointSpec = pow(max(dot(N, pointHalfwayDir), 0.0), shininess);
    vec3 pointSpecular = pointSpec * pointLightColor;
    float distance = length(pointLightPos - fragVert.xyz);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);
    float pointShadow = calculatePointShadow(fragVert.xyz);
    vec3 pointResult = (pointDiffuse + pointSpecular) * attenuation * pointShadow;

    // Końcowe połączenie
    vec3 finalLight = ambient + directionalResult + pointResult;
    vec3 result = finalLight * texColor.rgb;

    finalColor = vec4(result, texColor.a);
}

// Funkcja dla cienia kierunkowego
float calculateDirShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    float bias = 0.005;
    float radius = 1.5 / shadowMapSize;
    for (int i = 0; i < 16; i++) {
        shadow += texture(shadowMap, vec3(projCoords.xy + poissonDisk[i] * radius, currentDepth - bias));
    }
    shadow /= 16.0;
    if(projCoords.z > 1.0) shadow = 1.0;
    return shadow;
}

// Funkcja dla cienia punktowego
float calculatePointShadow(vec3 fragPos) {
    vec3 fragToLight = fragPos - pointLightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 4; // Ilość próbek w każdej z 3 osi
    float radius = 0.05; // Promień próbkowania w przestrzeni świata

    for(int x = -samples; x <= samples; ++x) {
        for(int y = -samples; y <= samples; ++y) {
            for(int z = -samples; z <= samples; ++z) {
                // Oblicz znormalizowaną głębokość z mapy (wartości od 0 do 1)
                float closestDepth = texture(shadowCubeMap, fragToLight + vec3(x, y, z) * radius).r;
                // Przeskaluj ją z powrotem do jednostek świata
                closestDepth *= far_plane_point;
                // Porównaj z aktualną głębokością
                if(currentDepth - bias > closestDepth) {
                    shadow += 1.0;
                }
            }
        }
    }
    float totalSamples = pow(float(samples) * 2.0 + 1.0, 3.0);
    shadow /= totalSamples;

    return 1.0 - shadow;
}