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
uniform uMaterial_t uMaterial;

uniform vec3 viewPos;

in vec2 oTexCoord;
in vec4 oColor;
in vec3 oNormal;

in vec3 oFragPos;

uniform vec3 uSunDirection;
uniform vec4 uSunColor;
uniform vec4 uAmbient;

void main() {
    // ambient
    vec3 ambient = uAmbient.rgb + uMaterial.ambient.rgb;
    // diffuse 
    vec3 norm = normalize(oNormal);
    vec3 lightDir = uSunDirection;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uSunColor.rgb * (diff * uMaterial.diffuse.rgb);
    if(uMaterial.hasDiffuse) {
        vec4 col = texture(uMaterial.diffuseTexture, oTexCoord);
        if(col.a < .5) discard;
        diffuse *= col.rgb;
    }
    
    // specular
    vec3 viewDir = normalize(viewPos - oFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uSunColor.rgb * (spec * uMaterial.specular.rgb);  
        
    vec3 result = ambient + diffuse + specular;
    fragColor = vec4(result, 1.0);
}