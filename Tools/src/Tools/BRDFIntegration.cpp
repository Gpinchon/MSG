#include <Tools/BRDFIntegration.hpp>

#include <glm/glm.hpp>

/**
 * sources :
 * Graphics Programming - Image-based Lighting : https://www.mathematik.uni-marburg.de/~thormae/lectures/graphics1/graphics_10_2_eng_web.html#1
 * KHR_materials_sheen                         : https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_sheen/README.md
 */

namespace TabGraph::Tools::BRDFIntegration {
constexpr auto sampleCount = 256;
float RadicalInverse_VdC(unsigned int bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

glm::vec2 Hammersley(unsigned int i, unsigned int N)
{
    return glm::vec2(float(i) / float(N), RadicalInverse_VdC(i));
}

float G1_GGX_Schlick(float NdotV, float roughness)
{
    float r = roughness; // original
    // float r = 0.5 + 0.5 * roughness; // Disney remapping
    float k     = (r * r) / 2.0;
    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float G_Smith(float NdotV, float NoL, float roughness)
{
    float g1_l = G1_GGX_Schlick(NoL, roughness);
    float g1_v = G1_GGX_Schlick(NdotV, roughness);
    return g1_l * g1_v;
}

float l(float x, float alpha_g)
{
    float one_minus_alpha_sq = (1.0 - alpha_g) * (1.0 - alpha_g);
    float a                  = glm::mix(21.5473, 25.3245, one_minus_alpha_sq);
    float b                  = glm::mix(3.82987, 3.32435, one_minus_alpha_sq);
    float c                  = glm::mix(0.19823, 0.16801, one_minus_alpha_sq);
    float d                  = glm::mix(-1.97760, -1.27393, one_minus_alpha_sq);
    float e                  = glm::mix(-4.32054, -4.85967, one_minus_alpha_sq);
    return a / (1.0 + b * pow(x, c)) + d * x + e;
}

float lambda_sheen(float cos_theta, float alpha_g)
{
    return abs(cos_theta) < 0.5 ? exp(l(cos_theta, alpha_g)) : exp(2.0 * l(0.5, alpha_g) - l(1.0 - cos_theta, alpha_g));
}

glm::vec2 IntegrateBRDF(float roughness, float NdotV, Type a_Type)
{
    glm::vec3 V;
    V.x              = sqrt(1.0 - NdotV * NdotV); // sin
    V.y              = 0.0;
    V.z              = NdotV; // cos
    glm::vec2 result = glm::vec2(0.0);
    for (uint n = 1u; n <= sampleCount; n++) {
        glm::vec2 p = Hammersley(n, sampleCount);
        float a     = roughness * roughness;
        float theta = acos(sqrt((1.0 - p.y) / (1.0 + (a * a - 1.0) * p.y)));
        float phi   = 2.0 * M_PI * p.x;
        // sampled h direction in normal space
        glm::vec3 H = glm::vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        glm::vec3 L = 2.0f * dot(V, H) * H - V;

        // because N = vec3(0.0, 0.0, 1.0) follows
        float NdotL = glm::clamp(L.z, 0.0f, 1.0f);
        float NdotH = glm::clamp(H.z, 0.0f, 1.0f);
        float VdotH = glm::clamp(dot(V, H), 0.0f, 1.0f);
        if (NdotL > 0.0) {
            float G_Vis = 0;
            if (a_Type == Type::Standard) {
                const float G = G_Smith(NdotV, NdotL, roughness);
                G_Vis         = G * VdotH / (NdotH * NdotV);
            } else if (a_Type == Type::Sheen) {
                G_Vis = 1.0 / ((1.0 + lambda_sheen(NdotV, roughness) + lambda_sheen(NdotL, roughness)) * (4.0 * NdotV * NdotL));
            }
            float Fc = pow(1.0 - VdotH, 5.0);
            result.x += (1.0 - Fc) * G_Vis;
            result.y += Fc * G_Vis;
        }
    }
    return result / float(sampleCount);
}

Pixels Generate(unsigned a_Width, unsigned a_Height, Type a_Type)
{
    Pixels pixels;
    for (auto y = 0; y < a_Width; ++y) {
        float yCoord = (y + 0.5f) / float(a_Width);
        for (auto x = 0; x < a_Height; ++x) {
            float xCoord       = (x + 0.5f) / float(a_Height);
            float NdotV        = xCoord;
            float roughness    = yCoord;
            const auto brdfVal = IntegrateBRDF(roughness, NdotV, a_Type);
            pixels[x][y][0]    = std::byte(brdfVal.x * 255);
            pixels[x][y][0]    = std::byte(brdfVal.y * 255);
        }
    }
    return pixels;
}
}