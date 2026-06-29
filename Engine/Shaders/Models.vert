#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoord;

uniform mat4 uTransform;
uniform mat4 uViewProjection;

out vec2 fragTextureCoord;

void main(){
    vec4 pos = vec4(inPos, 1.0);

    gl_Position = uViewProjection * uTransform * pos;
    fragTextureCoord = inTextureCoord;
}