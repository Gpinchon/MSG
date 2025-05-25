#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace MSG {
struct ImageInfo;
}

namespace MSG {
class ImageStorage {
public:
    ImageStorage(const ImageInfo& a_Info);
    ~ImageStorage();
    void Map(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size);
    void Unmap();
    glm::vec4 Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord);
    void Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color);

private:
    size_t _pageID;
    size_t _mappedByteOffset             = 0;
    std::vector<std::byte>* _mappedBytes = nullptr;
};
}