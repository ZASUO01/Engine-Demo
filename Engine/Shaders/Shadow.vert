#version 330 core

layout (location = 0) in vec3 inPos;

uniform mat4 uLightSpace;
uniform mat4 uTransform;

void main(){
    vec4 pos = vec4(inPos, 1.0);
    gl_Position = uLightSpace * uTransform * pos;
}