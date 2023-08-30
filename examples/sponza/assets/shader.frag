#version 460 core
out vec4 fragColor;

struct material_t {
    bool exist;

    vec3 albedo;
    float metallic;
    float roughness;

    sampler2D albedoTex;
    sampler2D metallicTex;
    sampler2D roughnessTex;
};
uniform material_t uMaterials[8];


in VS_OUT {
    vec3 normal;
    vec2 texCoord;
    float matID;
    vec4 color;
    vec3 fragPos;
    vec3 lightFragPos;
} fs_in;

uniform vec3 uViewPos;
uniform vec3 uSunDirection;
uniform vec3 uSunColor;
uniform vec3 uAmbient;

void main() {
    // fallback
    if(!uMaterials[int(fs_in.matID)].exist) {
        fragColor = vec4(1, 0, 1, 1);
        return;
    }

    // ambient
    vec3 ambient = uAmbient * .30;
    // diffuse 
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = uSunDirection;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uSunColor * (diff * uMaterials[int(fs_in.matID)].albedo);
    diffuse *= texture(uMaterials[int(fs_in.matID)].albedoTex, fs_in.texCoord).rgb;
    
    // specular
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterials[int(fs_in.matID)].metallic);
    vec3 specular = uSunColor * spec;  
        
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}