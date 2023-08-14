#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aColor;

out vec4 oColor;
out vec3 oNormal;
out vec2 oTexCoord;
out vec3 oFragPos;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;

void main() {
    oFragPos = (uModel * vec4(aPos, 1)).xyz;
    gl_Position = uProj * uView * vec4(oFragPos, 1.f);
    
    oColor = aColor;
    oTexCoord = aTexCoord;
    oNormal = aNormal;

}