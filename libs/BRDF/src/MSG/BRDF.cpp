#include <MSG/BRDF.hpp>
#include <MSG/Image.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/Halton.hpp>

#include <glm/geometric.hpp>

#include <numbers>

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a     = roughness;
    float k     = (a * a) / 2.f;
    float nom   = NdotV;
    float denom = NdotV * (1.f - k) + k;
    return nom / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

auto l(float x, float alpha_g)
{
    auto one_minus_alpha_sq = (1.0 - alpha_g) * (1.0 - alpha_g);
    auto a                  = glm::mix(21.5473, 25.3245, one_minus_alpha_sq);
    auto b                  = glm::mix(3.82987, 3.32435, one_minus_alpha_sq);
    auto c                  = glm::mix(0.19823, 0.16801, one_minus_alpha_sq);
    auto d                  = glm::mix(-1.97760, -1.27393, one_minus_alpha_sq);
    auto e                  = glm::mix(-4.32054, -4.85967, one_minus_alpha_sq);
    return a / (1.0 + b * pow(x, c)) + d * x + e;
}

auto lambda_sheen(float cos_theta, float alpha_g)
{
    return std::abs(cos_theta) < 0.5f ? exp(l(cos_theta, alpha_g)) : exp(2.f * l(0.5f, alpha_g) - l(1.f - cos_theta, alpha_g));
}

template <size_t Samples>
glm::vec2 IntegrateBRDFStandard(float roughness, float NdotV)
{
    const glm::vec3 V(
        sqrt(1.0 - NdotV * NdotV),
        0.f,
        NdotV);
    const glm::vec3 N(0.0, 0.0, 1.0);
    glm::vec2 result = { 0, 0 };
    for (uint32_t n = 0u; n < Samples; n++) {
        const auto Xi = MSG::Tools::Halton23<Samples>(n);
        const auto H  = MSG::BRDF::ImportanceSampleGGX(Xi, N, roughness);
        const auto L  = glm::normalize(2.f * dot(V, H) * H - V);
        float NdotL   = glm::max(L.z, 0.f);
        float NdotH   = glm::max(H.z, 0.f);
        float VdotH   = glm::max(dot(V, H), 0.f);
        if (NdotL <= 0.0)
            continue;
        const float Fc    = pow(1.f - VdotH, 5.f);
        const float G     = GeometrySmith(NdotV, NdotL, roughness);
        const float G_Vis = (G * VdotH) / (NdotH * NdotV);
        result += glm::vec2(
            (1.0 - Fc) * G_Vis,
            Fc * G_Vis);
    }
    return result / float(Samples);
}

template <size_t Samples>
glm::vec2 IntegrateBRDFSheen(float roughness, float NdotV)
{
    const glm::vec3 V(
        sqrt(1.0 - NdotV * NdotV),
        0.f,
        NdotV);
    const glm::vec3 N(0.0, 0.0, 1.0);
    glm::vec2 result = { 0, 0 };
    for (uint32_t n = 0u; n < Samples; n++) {
        const auto Xi     = MSG::Tools::Halton23<Samples>(n);
        const auto H      = MSG::BRDF::ImportanceSampleGGX(Xi, N, roughness);
        const auto L      = glm::normalize(2.f * dot(V, H) * H - V);
        const float NdotL = glm::max(L.z, 0.f);
        const float VdotH = glm::max(dot(V, H), 0.f);
        if (NdotL <= 0.0)
            continue;
        const float Fc    = pow(1.f - VdotH, 5.f);
        const float G_Vis = 1.f / float((1.f + lambda_sheen(NdotV, roughness) + lambda_sheen(NdotL, roughness)) * (4.f * NdotV * NdotL));
        result += glm::vec2(
            (1.0 - Fc) * G_Vis,
            Fc * G_Vis);
    }
    return result / float(Samples);
}

template <size_t Samples>
glm::vec2 IntegrateBRDF(float roughness, float NdotV, MSG::BRDF::Type a_Type)
{
    if (a_Type == MSG::BRDF::Type::Standard) {
        return IntegrateBRDFStandard<Samples>(roughness, NdotV);
    } else if (a_Type == MSG::BRDF::Type::Sheen) {
        return IntegrateBRDFSheen<Samples>(roughness, NdotV);
    }
    return {};
}

glm::vec3 MSG::BRDF::ImportanceSampleGGX(const glm::vec2& a_Xi, const glm::vec3& a_N, const float& a_Roughness)
{
    const auto a        = a_Roughness * a_Roughness;
    const auto Phi      = 2 * std::numbers::pi * a_Xi.x;
    const auto CosTheta = sqrt((1.f - a_Xi.y) / (1 + (a * a - 1) * a_Xi.y));
    const auto SinTheta = sqrt(1.f - CosTheta * CosTheta);
    const glm::vec3 H   = {
        SinTheta * cos(Phi),
        SinTheta * sin(Phi),
        CosTheta
    };

    const glm::vec3 UpVector = std::abs(a_N.z) < 0.999 ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
    const glm::vec3 TangentX = glm::normalize(glm::cross(UpVector, a_N));
    const glm::vec3 TangentY = glm::cross(a_N, TangentX);

    return glm::normalize(TangentX * H.x + TangentY * H.y + a_N * H.z);
}

glm::vec2 MSG::BRDF::IntegrateBRDF(const float& roughness, const float& NdotV, const Type& a_Type)
{
    if (a_Type == MSG::BRDF::Type::Standard) {
        return IntegrateBRDFStandard<64>(roughness, NdotV);
    } else if (a_Type == MSG::BRDF::Type::Sheen) {
        return IntegrateBRDFSheen<64>(roughness, NdotV);
    }
    return {};
}

MSG::Image MSG::BRDF::GenerateImage(const Type& a_Type, const uint32_t& a_Width, const uint32_t& a_Height)
{
    Image pixels(PixelSizedFormat::Uint8_NormalizedRG, a_Width, a_Height, 1);
    pixels.Allocate();
    for (auto y = 0u; y < a_Height; ++y) {
        const float roughness = y / float(a_Height - 1);
        for (auto x = 0u; x < a_Width; ++x) {
            const float NdotV = (x + 1) / float(a_Width);
            pixels.Store({ x, y, 0 }, MSG::PixelColor(IntegrateBRDF(roughness, NdotV, a_Type), 0, 0));
        }
    }
    return pixels;
}

MSG::Texture MSG::BRDF::GenerateTexture(const Type& a_Type, const uint32_t& a_Width, const uint32_t& a_Height)
{
    return { TextureType::Texture2D, std::make_shared<Image>(GenerateImage(a_Type, a_Width, a_Height)) };
}
