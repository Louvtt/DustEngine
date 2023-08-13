#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aColor;

out vec4 oColor;

uniform mat4 uView;
uniform mat4 uProj;

void main() {
    gl_Position = uView * uProj * vec4(aPos, 1.f);
    oColor = aColor;
}