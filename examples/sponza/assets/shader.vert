#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec4 aColor;
layout (location = 5) in float aMatID;

out VS_OUT {
    vec3 normal;
    vec2 texCoord;
    float matID;
    vec4 color;
    vec3 fragPos;
    vec3 tangent;
    vec3 lightFragPos;
} vs_out;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;
uniform mat4 uLightViewProj;

void main() {
    vs_out.fragPos = (uModel * vec4(aPos, 1)).xyz;
    gl_Position = uProj * uView * vec4(vs_out.fragPos, 1.f);
    
    vs_out.normal   = (uModel * vec4(aNormal, 0)).xyz;
    vs_out.tangent  = (uModel * vec4(aTangent, 0)).xyz;
    vs_out.color    = aColor;
    vs_out.texCoord = aTexCoord;
    vs_out.matID    = aMatID;
    vs_out.lightFragPos = (uLightViewProj * vec4(vs_out.fragPos, 1)).xyz;

}