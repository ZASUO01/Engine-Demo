#version 330 core

in vec2 fragTextureCoord;

uniform sampler2D uTexture;

out vec4 outColor;

void main(){
    outColor = texture(uTexture, fragTextureCoord);
}