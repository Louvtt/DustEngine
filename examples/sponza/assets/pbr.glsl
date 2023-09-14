#version 460 core
/***********************************************/
#pragma vscode_glsllint_stage : vert
#pragma vertex_shader

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
    mat3 TBN;
} vs_out;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;
uniform mat4 uLightViewProj;

void main() {
    vs_out.fragPos = (uModel * vec4(aPos, 1)).xyz;
    gl_Position = uProj * uView * vec4(vs_out.fragPos, 1.f);
    
    vs_out.normal   = normalize(mat3(uModel) * aNormal);
    vs_out.tangent  = normalize(mat3(uModel) * aTangent);
    vs_out.color    = aColor;
    vs_out.texCoord = aTexCoord;
    vs_out.matID    = aMatID;
    vs_out.lightFragPos = (uLightViewProj * vec4(vs_out.fragPos, 1)).xyz;

    // Normal mapping matrix (TBN)
    vec3 biTangent = cross(vs_out.normal, vs_out.tangent);
    // TBN [Tangent Bitangent Normal] matrix
    vs_out.TBN = mat3(vs_out.tangent, biTangent, vs_out.normal);
}


/***********************************************/
/***********************************************/
#pragma vscode_glsllint_stage : frag
#pragma fragment_shader

out vec4 FragColor;

#define PI 3.1415926535897932384626433832795
#define ONE_OVER_PI 1 / PI

/***********************************************/
// Materials

#define MAT_COUNT 6
struct material_t {
    bool exist;

    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

    sampler2D texAlbedo;
    sampler2D texNormal;
    sampler2D texMetallic;
    sampler2D texRoughness;
    sampler2D texAO;

};
uniform material_t uMaterials[MAT_COUNT];

/***********************************************/
// Lights

#define MAX_LIGHTS_COUNT 10
struct light_t {
    int type;

    vec3 position;
    vec3 direction;
    vec3 color;
    float falloff;
};
uniform int uLightCount;
uniform light_t uLights[MAX_LIGHTS_COUNT];

/***********************************************/
// Globals

uniform sampler2D uEnvironnmentMap;
uniform vec3 uViewPos;
uniform float uExposure;

const float gamma = 2.2;
const float inv_gamma = 1./gamma;

/***********************************************/
// Input

in VS_OUT {
    vec3 normal;
    vec2 texCoord;
    float matID;
    vec4 color;
    vec3 fragPos;
    vec3 tangent;
    vec3 lightFragPos;
    mat3 TBN;
} fs_in;

/***********************************************/
// Function

// Normal/Bump mapping
vec3 calcBumpMapping()
{
    vec3 normal = texture(uMaterials[int(fs_in.matID)].texNormal, fs_in.texCoord).xyz;
    normal = normal * 2.0 - 1.0;   
    return normalize(fs_in.TBN * normal); 
}

/***********************************************/
// BRDF Functions

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

/** Fresnel */
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

/***********************************************/
// Main

void main() {
    // check if material is set otherwise print pink/magenta
    if(!uMaterials[int(fs_in.matID)].exist) {
        FragColor = vec4(1, 0, 1, 1);
        return;
    }


    // Request scene data
    vec3 N = calcBumpMapping();
    vec3 V = normalize(uViewPos - fs_in.fragPos);

    vec3 albedo     = texture(uMaterials[int(fs_in.matID)].texAlbedo,    fs_in.texCoord).rgb * uMaterials[int(fs_in.matID)].albedo    ;
    albedo = pow(albedo, vec3(gamma)); // HDR
    float metallic  = texture(uMaterials[int(fs_in.matID)].texMetallic,  fs_in.texCoord).r   * uMaterials[int(fs_in.matID)].metallic  ;
    float roughness = texture(uMaterials[int(fs_in.matID)].texRoughness, fs_in.texCoord).r   * uMaterials[int(fs_in.matID)].roughness ;
    float ao        = texture(uMaterials[int(fs_in.matID)].texAO,        fs_in.texCoord).r   * uMaterials[int(fs_in.matID)].ao        ;

    // Calculate lights
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < uLightCount; ++i) 
    {
        // calculate per light radiance
        light_t light = uLights[i];
        vec3 L = normalize(light.direction);
        vec3 H = normalize(V + L);
        float NdotL = max(dot(N, L), 0.0);        
        float distance    = length(L);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance     = light.color; // * attenuation;

        vec3 F0 = vec3(0.04); // TODO: add parameter for dieletric
        F0      = mix(F0, albedo, metallic);
        vec3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

        // Calculate specular and diffuse term
        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        
        kD *= 1.0 - metallic;	

        float NDF = DistributionGGX(N, H, roughness);       
        float G   = GeometrySmith(N, V, L, roughness);
        // Cook torrance BRDF
        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL  + 0.0001;
        vec3 specular     = numerator / denominator;

        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    // TODO: add parameter for ambient 'factor'
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color   = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(inv_gamma)); 

    FragColor = vec4(color, 1.0);
}