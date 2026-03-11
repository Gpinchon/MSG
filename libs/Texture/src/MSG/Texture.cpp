#include <MSG/Image.hpp>
#include <MSG/Texture.hpp>

template <typename T>
inline uint32_t GetMipCount(const T& a_BaseSize)
{
    typename T::value_type maxVal = 0;
    for (uint8_t i = 0; i < T::length(); i++)
        maxVal = glm::max(maxVal, a_BaseSize[i]);
    return maxVal == 0 ? 0 : floor(log2(maxVal) + 1);
}

Msg::Texture::Texture(const TextureType& a_Type)
    : Inherit()
{
    SetType(a_Type);
}

Msg::Texture::Texture(const TextureType& a_Type, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Size)
    : Texture(a_Type)
{
    SetPixelDescriptor(a_PixDesc);
    SetSize(a_Size);
    resize(GetMipCount(a_Size));
    for (uint32_t lvl = 0; lvl < size(); lvl++) {
        glm::uvec3 lvlSize = glm::max(glm::uvec3(1u), a_Size / uint32_t(exp2(lvl)));
        ImageInfo info;
        info.width     = lvlSize.x;
        info.height    = lvlSize.y;
        info.depth     = lvlSize.z;
        info.pixelDesc = GetPixelDescriptor();
        at(lvl)        = std::make_shared<Image>(info);
    }
}

Msg::Texture::Texture(const TextureType& a_Type, const std::shared_ptr<Image>& a_Image)
    : Texture(a_Type)
{
    SetPixelDescriptor(a_Image->GetPixelDescriptor());
    SetSize(a_Image->GetSize());
    emplace_back(a_Image);
}

glm::uvec3 Msg::Texture::GetSize(const uint32_t& a_Level) const
{
    return glm::max(GetSize() / uint32_t(exp2(a_Level)), 1u);
}
