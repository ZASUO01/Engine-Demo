#version 330 core

in vec3 fragPos;
in vec2 fragTextureCoord;
in vec3 fragNormal;
in vec4 fragLightSpacePos;

uniform sampler2D uTexture;
uniform sampler2D uShadowMap;
uniform vec3 uLightDir;

out vec4 outColor;

float ShadowCalculation(vec4 lightSpacePos, vec3 normal, vec3 lightDir) {
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;

    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;

            shadow += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
   vec3 color = texture(uTexture, fragTextureCoord).rgb;
       vec3 normal = normalize(fragNormal);
       vec3 lightDir = normalize(uLightDir);

       vec3 ambient = 0.2 * color;

       float diff = max(dot(lightDir, normal), 0.0);
       vec3 diffuse = 0.8 * diff * color;

       float shadow = ShadowCalculation(fragLightSpacePos, normal, lightDir);

       vec3 lighting = ambient + ((1.0 - shadow) * diffuse);

       outColor = vec4(lighting, 1.0);
}