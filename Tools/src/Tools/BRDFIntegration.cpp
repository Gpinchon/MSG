#include <Tools/BRDFIntegration.hpp>
#include <Tools/Halton.hpp>

#include <glm/glm.hpp>

/**
 * sources :
 * Graphics Programming - Image-based Lighting : https://www.mathematik.uni-marburg.de/~thormae/lectures/graphics1/graphics_10_2_eng_web.html#1
 * KHR_materials_sheen                         : https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_materials_sheen/README.md
 */

namespace TabGraph::Tools::BRDFIntegration {
glm::vec3 ImportanceSampleGGX(const glm::vec2& a_Xi, const glm::vec3& a_N, const float& a_Roughness)
{
    const float a        = a_Roughness * a_Roughness;
    const float Phi      = 2 * M_PI * a_Xi.x;
    const float CosTheta = sqrt((1.f - a_Xi.y) / (1 + (a * a - 1) * a_Xi.y));
    const float SinTheta = sqrt(1.f - CosTheta * CosTheta);
    const glm::vec3 H    = {
        SinTheta * cos(Phi),
        SinTheta * sin(Phi),
        CosTheta
    };

    const glm::vec3 UpVector = abs(a_N.z) < 0.999 ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
    const glm::vec3 TangentX = glm::normalize(glm::cross(UpVector, a_N));
    const glm::vec3 TangentY = glm::cross(a_N, TangentX);

    return glm::normalize(TangentX * H.x + TangentY * H.y + a_N * H.z);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
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

template <unsigned Size>
glm::vec2 Halton23(const unsigned& a_Index)
{
    constexpr auto halton2 = Tools::Halton<2>::Sequence<Size>();
    constexpr auto halton3 = Tools::Halton<3>::Sequence<Size>();
    const auto rIndex      = a_Index % Size;
    return { halton2[rIndex], halton3[rIndex] };
}

template <size_t Samples>
glm::vec2 IntegrateBRDF(float roughness, float NdotV, Type a_Type)
{
    const glm::vec3 V(
        sqrt(1.0 - NdotV * NdotV),
        0.f,
        NdotV);
    const glm::vec3 N(0.0, 0.0, 1.0);
    glm::vec2 result = { 0, 0 };
    for (uint n = 0u; n < Samples; n++) {
        const auto Xi = Halton23<Samples>(n);
        const auto H  = ImportanceSampleGGX(Xi, N, roughness);
        const auto L  = glm::normalize(2.f * dot(V, H) * H - V);

        float NdotL = glm::max(L.z, 0.f);
        float NdotH = glm::max(H.z, 0.f);
        float VdotH = glm::max(dot(V, H), 0.f);
        if (NdotL <= 0.0)
            continue;
        float Fc    = pow(1.0 - VdotH, 5.0);
        float G_Vis = 0;
        if (a_Type == Type::Standard) {
            float G = GeometrySmith(NdotV, NdotL, roughness);
            G_Vis   = (G * VdotH) / (NdotH * NdotV);
        } else if (a_Type == Type::Sheen) {
            G_Vis = 1.0 / ((1.0 + lambda_sheen(NdotV, roughness) + lambda_sheen(NdotL, roughness)) * (4.0 * NdotV * NdotL));
        }

        result += glm::vec2(
            (1.0 - Fc) * G_Vis,
            Fc * G_Vis);
    }
    return result / float(Samples);
}

Pixels Generate(unsigned a_Width, unsigned a_Height, Type a_Type)
{
    Pixels pixels(a_Width, std::vector<Color>(a_Height));
    for (auto y = 0; y < a_Height; ++y) {
        const float roughness = y / float(a_Height - 1);
        for (auto x = 0; x < a_Width; ++x) {
            const float NdotV = (x + 1) / float(a_Width);
            pixels[x][y]      = IntegrateBRDF<64>(roughness, NdotV, a_Type);
        }
    }
    return pixels;
}
}