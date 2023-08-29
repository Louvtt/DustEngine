#version 460 core
out vec4 fragColor;

struct uMaterial_t {
    sampler2D diffuseTexture;
    bool hasDiffuse;
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    float shininess;
};
uniform bool uHasMaterial;
uniform uMaterial_t uMaterial;

in VS_OUT {
    vec3 fragPos;
    vec2 texCoord;
    vec3 normal;
    vec4 fragPosLightSpace;
} fs_in;

uniform sampler2D uShadowMap;
uniform vec3 uLightDir;
uniform vec3 uLightColor;
uniform vec4 uAmbient;
uniform vec3 uViewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * .5 + .5; // remap to [0;1] from [-1;1]
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    // compute is in shadow
    vec3 normal = normalize(fs_in.normal);
    vec3 lightDir = uLightDir;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow; 
}

void main() {
    if(!uHasMaterial) {
        fragColor = vec4(1, 0, 1, 1);
        return;
    }

    vec3 ambient = uMaterial.ambient.rgb * uAmbient.rgb * (uLightColor * .3);
    // diffuse 
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = uLightDir;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLightColor.rgb * (diff * uMaterial.diffuse.rgb);
    if(uMaterial.hasDiffuse) {
        vec4 col = texture(uMaterial.diffuseTexture, fs_in.texCoord);
        if(col.a < 0.5) discard;
        diffuse *= col.rgb;
    }
    
    // specular
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = uLightColor * (spec * uMaterial.specular.rgb);  

    float shadow = ShadowCalculation(fs_in.fragPosLightSpace);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular));
    fragColor = vec4(result, 1.0);
}