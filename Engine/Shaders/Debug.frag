#version 330 core

uniform vec3 uColor;

out vec4 outColor;

void main(){
    outColor = vec4(uColor, 1.0);
}
