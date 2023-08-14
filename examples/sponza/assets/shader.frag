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

void main() {
    if(uMaterial.hasDiffuse) {
        fragColor = uMaterial.ambient * texture(uMaterial.diffuseTexture, oTexCoord);
    } else {
        fragColor = uMaterial.ambient;
    }

}