#version 460 core
out vec4 fragColor;

#define PI 3.1415926535897932384626433832795
#define ONE_OVER_PI 1 / PI

/***********************************************/
// Materials

#define MAT_COUNT 8
struct material_t {
    bool exist;

    vec3 albedo;
    vec3 ior;
    float roughness;
    float metallic;

    sampler2D texAlbedo;
    sampler2D texReflectance;
    sampler2D texEmissive;
    sampler2D texNormal;

};
// uniform sampler2D uMatAlbedo[MAT_COUNT];
// uniform sampler2D uMatReflectance[MAT_COUNT];
// uniform sampler2D uMatEmissive[MAT_COUNT];
// uniform sampler2D uMatNormal[MAT_COUNT];
uniform material_t uMaterials[MAT_COUNT];

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
uniform float uExposure;

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

vec3 lerp(vec3 a, vec3 b, float t)
{
    return ((1 - t) * a) + (b * t);
}

float lerp(float a, float b, float t)
{
    return ((1 - t) * a) + (b * t);
}

// Normal/Bump mapping
vec3 calcBumpMapping()
{
    vec3 normal = texture(uMaterials[int(fs_in.matID)].texNormal, fs_in.texCoord).xyz;
    normal = normal * 2.0 - 1.0;   
    return normalize(fs_in.TBN * normal); 
}

/***********************************************/
// BRDF Functions
// From : https://boksajak.github.io/files/CrashCourseBRDF.pdf

// Distribution term
// GGX/Trowbridge-Reitz Normal distribution function
float D(float alpha, vec3 N, vec3 H) {
    float numerator = alpha * alpha;

    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denominator = ((numerator - 1) * NdotH2) + 1;
    denominator = max(denominator * denominator, 0.000001);

    return numerator / (PI * denominator);
}

/** 
 * GGX/Schlick-Beckmann Geometry Shadowing Function
 *
 * @param alpha
 * @param N - microfacet normal
 * @param S - either L or V
 */
float Gi(float alpha, vec3 N, vec3 S)
{
    float alphaSquared = alpha * alpha;
    float NdotSSquared = dot(N, S);
    NdotSSquared *= NdotSSquared;
    return 2 / (sqrt(((alphaSquared * (1 - NdotSSquared)) + NdotSSquared) / NdotSSquared) + 1);
}

/**
 * Geometric Attenuation Term
 * Smith Model
 * @param alpha
 * @param N - normal
 * @param V - view Dir
 * @param L - light dir
 */
float G(float alpha, vec3 N, vec3 V, vec3 L)
{
    return Gi(alpha, N, L) * Gi(alpha, N, V);
}

/** 
 * Fresnel (Schlick's approximation)
 * @param F0  - Reflectance at normal incidence
 * @param F90 - Reflectance at 90 incidence (equal to 1.0)
 * @param V   - View direction
 * @param N   - Normal
 */
vec3 F(vec3 F0, vec3 F90, vec3 V, vec3 N)
{
    return F0 + (F90 - F0) * pow(1 - max(dot(V,N), 0), 5);
}

float luminance(vec3 rgb) {
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

// Frostbite's version of Disney diffuse with energy normalization.
// Source: "Moving Frostbite to Physically Based Rendering" by Lagarde & de Rousiers
float frostbiteDisneyDiffuse(vec3 N, vec3 L, vec3 H, vec3 V) {
    float LdotH = max(dot(L, H), 0);
    float NdotL = max(dot(N, L), 0);
    float NdotV = max(dot(N, V), 0);
    float roughness = uMaterials[int(fs_in.matID)].roughness;

	float energyBias = 0.5f * roughness;
	float energyFactor = lerp(1, 1 / 1.51, roughness);

	float FD90MinusOne = energyBias + 2.0 * LdotH * LdotH * roughness - 1.0;

	float FDL = 1.0 + (FD90MinusOne * pow(1.0 - NdotL, 5.0));
	float FDV = 1.0 + (FD90MinusOne * pow(1.0 - NdotV, 5.0));

	return FDL * FDV * energyFactor;
}

vec3 evalFrostbiteDisneyDiffuse(vec3 reflectance, vec3 N, vec3 L, vec3 H, vec3 V) {
    float NdotL = max(dot(N, L), 0);
	return reflectance * (frostbiteDisneyDiffuse(N, L, H, V) * ONE_OVER_PI * NdotL);
}

// Microfacet
vec3 evalCookTorance(vec3 reflectance, float alpha, vec3 color, float metalness, vec3 L, vec3 N, vec3 H, vec3 V)
{
    // Calculate color at normal incidence
    vec3 F0  = lerp((reflectance * reflectance * .16), color, metalness);
    vec3 F90 = vec3(min(1.60, luminance(F0)));
    // vec3 diffuse_reflectance = color.rgb * (1 - metalness);

    // Cook torrance
    float LdotN = max(dot(L, N), 0.0);
    vec3 cookTorranceNum  = F(F0, F90, V, H) * G(alpha, N, V, L) * D(alpha, N, H);
    float cookTorranceDen = 4. *  max(dot(V, N), 0.0) * LdotN;
    cookTorranceDen = max(cookTorranceDen, 0.000001);
    vec3 cookTorrance = (cookTorranceNum / cookTorranceDen) * LdotN;
    return cookTorrance;
}

vec3 PBR(vec3 V, vec3 N, vec3 L, vec3 H)
{
    // Mat param
    vec4  color     = texture(uMaterials[int(fs_in.matID)].texAlbedo, fs_in.texCoord) * vec4(uMaterials[int(fs_in.matID)].albedo, 1);
    float metalness = uMaterials[int(fs_in.matID)].metallic;
    float alpha     = uMaterials[int(fs_in.matID)].roughness * uMaterials[int(fs_in.matID)].roughness;
    vec3  ior       = uMaterials[int(fs_in.matID)].ior;
    vec3  reflectance = color.rgb * (1 - metalness);

    vec3 specular = evalCookTorance(reflectance, alpha, color.rgb, metalness, L, N, H, V);
    vec3 diffuse  = evalFrostbiteDisneyDiffuse(vec3(reflectance), N, L, H, V);

    // Combine with Fresnel
    vec3 F0 = lerp((reflectance * reflectance * .16), color.rgb, metalness);
    vec3 F90 = vec3(min(1.60, luminance(F0)));
    vec3 F  = F(F0, F90, V, N);

    vec3 emissive = vec3(0.0); //texture(uMaterials[int(fs_in.matID)].texEmissive, fs_in.texCoord);
    return ((vec3(1) - F) * diffuse + specular) + emissive;
}

/***********************************************/

void main() {
    // fallback
    if(!uMaterials[int(fs_in.matID)].exist) {
        fragColor = vec4(1, 0, 1, 1);
        return;
    }
    // fragColor = vec4(texture(uMaterials[int(fs_in.matID)].texAlbedo, fs_in.texCoord).rgb, 1);
    // return;

    // inputs
    vec3 normal = calcBumpMapping();                      // Normal
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);   // View vector
    // Lights
    vec3 lightDir = normalize(uLights[0].direction);      // Light vector
    vec3 halfView = normalize(viewDir + lightDir);        // Half View vector

    vec3 result = PBR(viewDir, normal, lightDir, halfView);
    fragColor = vec4(result, 1.0);

    // tonemapping
    const float gamma = 2.2;
    vec3 hdrColor = fragColor.xyz;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * uExposure);
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / gamma));
    // output
    fragColor = vec4(mapped, fragColor.a);
}