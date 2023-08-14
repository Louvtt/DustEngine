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

vec3 forward = vec3(-1, 0, 0);

void main() {
    
    if(uMaterial.hasDiffuse) {
        fragColor = uMaterial.diffuse * texture(uMaterial.diffuseTexture, oTexCoord);
    } else {
        fragColor = uMaterial.diffuse + uMaterial.ambient;
    }

    if(fragColor.a < .5) discard;

    // FlatShading
    float dir = dot(oNormal, forward);
    fragColor *= vec4(vec3(dir), 1);
}