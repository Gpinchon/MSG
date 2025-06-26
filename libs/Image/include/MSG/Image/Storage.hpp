#pragma once

#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#ifndef MSG_RAM_IMAGE_STORAGE
// #define MSG_RAM_IMAGE_STORAGE
#endif

namespace MSG {
class PageRef;
struct PixelDescriptor;
}

namespace MSG {
class ImageStorage {
public:
    ImageStorage() = default;
    ImageStorage(const size_t& a_ByteSize);
    ImageStorage(const std::vector<std::byte>& a_Data);
    ImageStorage(const ImageStorage& a_Src, const uint32_t& a_LayerOffset = 0u);

    void Allocate(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc);
    void Release();

    /**
     * @brief Reads a range of pixels from the storage
     * @return std::vector<std::byte>
     */
    std::vector<std::byte> Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size);
    /**
     * @brief Writes a range of pixels to the storage
     */
    void Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte> a_Data);

    /**
     * @brief Reads a range of pixels from the storage and store it for later read/write
     */
    void Map(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_Offset, const glm::uvec3& a_Size);
    /**
     * @brief Writes the mapped range of pixels back to the storage
     */
    void Unmap(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc);
    /**
     * @brief Reads a single pixel from the storage
     * @return glm::vec4
     */
    glm::vec4 Read(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord);
    /**
     * @brief Writes a single pixel to the storage
     */
    void Write(const glm::uvec3& a_ImageSize, const PixelDescriptor& a_PixDesc, const glm::uvec3& a_TexCoord, const glm::vec4& a_Color);

private:
    uint32_t _layerOffset = 0u;
#ifdef MSG_RAM_IMAGE_STORAGE
    std::shared_ptr<std::vector<std::byte>> _data;
#else
    std::shared_ptr<PageRef> _pageRef;
    glm::uvec3 _modifiedBeg  = glm::uvec3(-1u);
    glm::uvec3 _modifiedEnd  = glm::uvec3(0u);
    glm::uvec3 _mappedOffset = glm::uvec3(-1u);
    glm::uvec3 _mappedSize   = glm::uvec3(0u);
    std::vector<std::byte> _mappedBytes;
#endif
};
}