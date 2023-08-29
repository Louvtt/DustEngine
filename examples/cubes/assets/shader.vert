#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT {
    vec3 fragPos;
    vec2 texCoord;
    vec3 normal;
    vec4 fragPosLightSpace;
} vs_out;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;
uniform mat4 uLightViewProj;

void main() {
    vs_out.fragPos = vec3(uModel * vec4(aPos, 1));
    gl_Position = uProj * uView * vec4(vs_out.fragPos, 1);
    
    vs_out.texCoord = aTexCoord;
    vs_out.normal   = transpose(inverse(mat3(uModel))) * aNormal;
    vs_out.fragPosLightSpace = uLightViewProj * vec4(vs_out.fragPos, 1);
}