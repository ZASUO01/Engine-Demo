#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoord;

uniform mat4 uViewProjection;
uniform mat4 uTransform;
uniform mat4 uLightSpace;

out vec3 fragPos;
out vec2 fragTextureCoord;
out vec3 fragNormal;
out vec4 fragLightSpacePos;

void main(){
    vec4 worldPos = uTransform * vec4(inPos, 1.0);

    fragPos = worldPos.xyz;
    fragTextureCoord = inTextureCoord;
    fragNormal = mat3(transpose(inverse(uTransform))) * inNormal;
    fragLightSpacePos = uLightSpace * worldPos;

    gl_Position = uViewProjection * worldPos;
}