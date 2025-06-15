#version 330 core

// --- Dane wejĹ›ciowe ---
in vec4 fragColor;
in vec2 fragTexCoord;
in vec4 fragNormal;
in vec4 fragVert;
in vec4 fragPosLightSpace;

// --- WyjĹ›cie ---
out vec4 finalColor;

// --- Tekstury ---
uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2DShadow shadowMap;

// --- Uniformy ogĂłlne ---
uniform float shadowMapSize;
uniform vec3 viewPos; // Pozycja kamery

// --- Uniformy dla Ĺ›wiatĹ‚a kierunkowego (sĹ‚oĹ„ce) ---
uniform vec3 directionalLightPos;

// --- Uniformy dla Ĺ›wiatĹ‚a punktowego ---
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float linear;
uniform float quadratic;

// Funkcja obliczajÄ…ca cieĹ„ (bez zmian)
float calculateShadow(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    float bias = 0.005;
    float radius = 1.5 / shadowMapSize;
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
    // --- WspĂłlne zmienne oĹ›wietlenia ---
    vec3 N = normalize(fragNormal.xyz);
    vec3 viewDir = normalize(viewPos - fragVert.xyz);
    vec4 texColor = texture(textureMap0, fragTexCoord);
    float shininess = 32.0; // PoĹ‚yskliwoĹ›Ä‡ materiaĹ‚u

    // --- 1. Obliczenia dla Ĺ›wiatĹ‚a kierunkowego (sĹ‚oĹ„ce) ---
    vec3 directionalLightColor = vec3(1.0, 1.0, 0.95);
    vec3 ambient = 0.3 * directionalLightColor; // ĹšwiatĹ‚o otoczenia pochodzi tylko od sĹ‚oĹ„ca
    vec3 lightDir = normalize(directionalLightPos - fragVert.xyz);

    // Komponent rozproszony (Diffuse)
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = diff * directionalLightColor;
    
    // Komponent lustrzany (Specular) - Blinn-Phong
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(N, halfwayDir), 0.0), shininess);
    vec3 specular = spec * directionalLightColor;
    
    // Oblicz cieĹ„
    float shadow = calculateShadow(fragPosLightSpace);
    
    // PoĹ‚Ä…cz komponenty dla Ĺ›wiatĹ‚a kierunkowego (tylko diffuse i specular sÄ… w cieniu)
    vec3 directionalResult = (diffuse + specular) * shadow;

    // --- 2. Obliczenia dla Ĺ›wiatĹ‚a punktowego ---
    vec3 pointLightDir = normalize(pointLightPos - fragVert.xyz);

    // Komponent rozproszony
    float pointDiff = max(dot(N, pointLightDir), 0.0);
    vec3 pointDiffuse = pointDiff * pointLightColor;
    
    // Komponent lustrzany
    vec3 pointHalfwayDir = normalize(pointLightDir + viewDir);
    float pointSpec = pow(max(dot(N, pointHalfwayDir), 0.0), shininess);
    vec3 pointSpecular = pointSpec * pointLightColor;
    
    // TĹ‚umienie (Attenuation)
    float distance = length(pointLightPos - fragVert.xyz);
    float attenuation = 1.0 / (1.0 + linear * distance + quadratic * distance * distance);
    
    // PoĹ‚Ä…cz komponenty dla Ĺ›wiatĹ‚a punktowego (z tĹ‚umieniem)
    vec3 pointResult = (pointDiffuse + pointSpecular) * attenuation;

    // --- 3. KoĹ„cowe poĹ‚Ä…czenie ---
    // Zsumuj Ĺ›wiatĹ‚o otoczenia i wpĹ‚yw obu ĹşrĂłdeĹ‚ Ĺ›wiatĹ‚a, a nastÄ™pnie pomnĂłĹĽ przez kolor tekstury
    vec3 finalLight = ambient + directionalResult + pointResult;
    vec3 result = finalLight * texColor.rgb;

    finalColor = vec4(result, texColor.a);
}