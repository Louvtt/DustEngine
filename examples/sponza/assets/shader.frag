#version 460 core
out vec4 fragColor;

struct material_t {
    sampler2D diffuseTexture;
    bool hasDiffuse;
    vec4 diffuse;
    vec4 ambient;
};
uniform material_t uMaterial;

in vec2 oTexCoord;
in vec4 oColor;
in vec3 oNormal;

uniform vec3 uSunDirection;
uniform vec4 uSunColor;
uniform vec4 uAmbient;

void main() {
    float shadow = max(dot(oNormal, uSunDirection), 0.0);
    vec4 diffuse = uMaterial.diffuse;
    if(uMaterial.hasDiffuse) {
        diffuse *= texture(uMaterial.diffuseTexture, oTexCoord);
    }
    if(diffuse.a < .5) discard;

    vec4 ambient = (shadow * uSunColor) + uAmbient;
    fragColor = (diffuse + ambient);
}