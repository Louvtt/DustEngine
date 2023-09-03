#version 460 core
out vec4 fragColor;

#define PI 3.1415926535897932384626433832795

/***********************************************/
// Materials

struct material_t {
    bool exist;

    vec3 albedo;
    vec3 ior;
    float roughness;
    float metallic;

    sampler2D albedoTex;
    sampler2D reflectanceTex;
    sampler2D emissiveTex;
    sampler2D normalTex;
};
uniform material_t uMaterials[8];

/***********************************************/
// Lights

struct light_t {
    int type;

    vec3 position;
    vec3 direction;
    vec3 color;
    float falloff;
};
uniform int uLightCount;
uniform light_t uLights[10];

/***********************************************/
// Globals

uniform sampler2D uEnvironnmentMap;
uniform vec3 uViewPos;

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
} fs_in;

/***********************************************/
// Functions

vec3 CalcNormal()
{
    vec3 normal = normalize(fs_in.normal);
    vec3 tangent = normalize(fs_in.tangent);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 biTangent = cross(tangent, normal);
    vec3 bumpMapNormal = texture(uMaterials[int(fs_in.matID)].normalTex, fs_in.texCoord).xyz;
    bumpMapNormal = 2.0 * bumpMapNormal - vec3(1);
    mat3 TBN = mat3(tangent, biTangent, normal);
    return normalize(TBN * bumpMapNormal);
}

// GGX/Trowbridge-Reitz Normal distribution function
float D(float alpha, vec3 N, vec3 H) {
    float numerator = alpha * alpha;

    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denominator = NdotH2 * (numerator - 1.0) + 1.0;
    denominator = max(denominator * denominator, 0.000001);

    return numerator / (PI * denominator);
}

// GGX/Schlick-Beckmann Geometry Shadowing Function
float Gi(float alpha, vec3 N, vec3 X)
{
    float numerator = max(dot(N, X), 0.0);

    float k = alpha * .5;
    float denominator = numerator * (1.0 - k) + k;
    denominator = max(denominator, 0.000001);

    return numerator / denominator;
}

// Smith Model
float G(float alpha, vec3 N, vec3 V, vec3 L)
{
    return Gi(alpha, N, V) * Gi(alpha, N, L);
}

// Fresnel
vec3 F(vec3 F0, vec3 V, vec3 H)
{
    return F0 + (vec3(1) - F0) * pow(1 - max(dot(V,H), 0.0), 5);
}

vec3 lerp(vec3 a, vec3 b, float t)
{
    return ((1 - t) * a) + (b * t);
}

vec3 PBR(vec3 V, vec3 N, vec3 L, vec3 H)
{
    // Mat param
    vec4 color = texture(uMaterials[int(fs_in.matID)].albedoTex, fs_in.texCoord);
    float metallic = uMaterials[int(fs_in.matID)].metallic;
    float alpha = uMaterials[int(fs_in.matID)].roughness;

    // Calculate color at normal incidence
    vec3 ior = uMaterials[int(fs_in.matID)].ior;
    vec3 F0 = abs((1.0 - ior) / (1.0 + ior));
    F0 = F0 * F0;
    F0 = lerp(F0, color.rgb, metallic);

    // Power Conservation
    vec3 Ks = F(F0, V, H);
    vec3 Kd = (vec3(1) - Ks) * (1.0 - uMaterials[int(fs_in.matID)].metallic);

    vec3 lambert = color.rgb / PI;
    
    float LdotN = max(dot(L, N), 0.0);
    vec3 cookTorranceNum  = D(alpha, N, H) * G(alpha, N, V, L) * F(F0, V, H);
    float cookTorranceDen = 4. *  max(dot(V, N), 0.0) * LdotN;
    cookTorranceDen = max(cookTorranceDen, 0.000001);
    vec3 cookTorrance = cookTorranceNum / cookTorranceDen;

    vec3 BRDF = Kd * lambert * cookTorrance;
    vec3 emissivity = texture(uMaterials[int(fs_in.matID)].reflectanceTex, fs_in.texCoord).rgb;
    vec3 res = emissivity + BRDF * uLights[0].color * LdotN;

    return res;
}

/***********************************************/

void main() {
    // fallback
    if(!uMaterials[int(fs_in.matID)].exist) {
        fragColor = vec4(1, 0, 1, 1);
        return;
    }

    // inputs
    vec3 N = CalcNormal();                          // Normal
    vec3 V = normalize(uViewPos - fs_in.fragPos);   // View vector
    // Lights
    vec3 L = normalize(uLights[0].direction);       // Light vector
    vec3 H = normalize(V + L);                      // Half View vector

    // ambient
    vec3 result = PBR(V, N, L, H);
    fragColor = vec4(result, 1.0);
}