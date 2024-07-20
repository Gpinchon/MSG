#include <Tools/GGXLUT.hpp>

namespace TabGraph::Tools::GGXLUT {
glm::vec2 LightingFuncGGX_FV(float LdotH, float alpha)
{
    // F
    float LdotH5 = pow(1.0f - LdotH, 5);
    float F_a    = 1.0f;
    float F_b    = LdotH5;
    // V
    float k     = alpha / 2.0f;
    float k2    = k * k;
    float invK2 = 1.0f - k2;
    float vis   = 1 / (LdotH * LdotH * invK2 + k2);
    return { F_a * vis, F_b * vis };
}

float LightingFuncGGX_D(float NdotH, float alpha)
{
    float alphaSqr = alpha * alpha;
    float pi       = 3.14159f;
    float denom    = NdotH * NdotH * (alphaSqr - 1.0f) + 1.0f;
    return alphaSqr / (pi * denom * denom);
}

Pixels GenerateFV(unsigned a_Width, unsigned a_Height)
{
    Pixels pixels(a_Width, std::vector<Color>(a_Height));
    for (uint y = 0; y < a_Height; ++y) {
        float alpha = y / float(a_Height);
        for (uint x = 0; x < a_Width; ++x) {
            float LdotH  = x / float(a_Width);
            pixels[x][y] = Color(LightingFuncGGX_FV(LdotH, alpha), 0);
        }
    }
    return pixels;
}
Pixels GenerateD(unsigned a_Width, unsigned a_Height)
{
    Pixels pixels(a_Width, std::vector<Color>(a_Height));
    for (uint y = 0; y < a_Height; ++y) {
        float alpha = y / float(a_Height);
        for (uint x = 0; x < a_Width; ++x) {
            float NdotH  = x / float(a_Width);
            pixels[x][y] = Color(LightingFuncGGX_D(NdotH, alpha));
        }
    }
    return pixels;
}

Pixels GenerateFVD(unsigned a_Width, unsigned a_Height)
{
    Pixels pixels(a_Width, std::vector<Color>(a_Height));
    for (uint y = 0; y < a_Height; ++y) {
        float alpha = y / float(a_Height);
        for (uint x = 0; x < a_Width; ++x) {
            float LdotH  = x / float(a_Width);
            float NdotH  = x / float(a_Width);
            pixels[x][y] = Color(
                LightingFuncGGX_FV(LdotH, alpha),
                LightingFuncGGX_D(NdotH, alpha));
        }
    }
    return pixels;
}
}