#version 330 core

in vec2 fragTextureCoord;

uniform sampler2D uTexture;
uniform vec3 uColor;
uniform float uTextureFactor;

out vec4 outColor;

void main(){
    vec4 color = vec4(uColor, 1.0);
    vec4 textureColor = texture(uTexture, fragTextureCoord);

    vec4 tintedTexture = color * textureColor;

    outColor = mix(color, tintedTexture, uTextureFactor);
}