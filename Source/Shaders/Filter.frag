#version 330 core

in vec2 fragTextureCoord;

uniform sampler2D uScreenTexture;

out vec4 outColor;

void main(){
    vec2 dir = fragTextureCoord - vec2(0.5);
    float dist = length(dir);

    float offset = 0.003;
    float aberrationAmount = dist * offset;

    float r = texture(uScreenTexture, fragTextureCoord + vec2(aberrationAmount, 0.0)).r;
    float g = texture(uScreenTexture, fragTextureCoord).g;
    float b = texture(uScreenTexture, fragTextureCoord - vec2(aberrationAmount, 0.0)).b;
    vec3 color = vec3(r, g, b);

    color = color / (color + vec3(1.0));

    float gamma = 2.2;
    color = pow(color, vec3(1.0 / gamma));

    float vignette = smoothstep(0.8, 0.2, dist);
    color *= vignette;

    outColor = vec4(color, 1.0);
}
