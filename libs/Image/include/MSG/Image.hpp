#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/ImageStorage.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <glm/fwd.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
struct ImageInfo {
    uint32_t width  = 1;
    uint32_t height = 1;
    uint32_t depth  = 1;
    PixelDescriptor pixelDesc;
    ImageStorage storage;
};

class Image : public Core::Inherit<Core::Object, Image> {
public:
    PROPERTY(PixelDescriptor, PixelDescriptor, );
    PROPERTY(glm::uvec3, Size, 0);

public:
    Image();
    Image(const ImageInfo& a_Info);

    auto& GetStorage() { return _storage; }
    auto& GetStorage() const { return _storage; }
    void SetStorage(const ImageStorage& a_Storage) { _storage = a_Storage; }

    void Write(const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte>&& a_Data);
    void Write(std::vector<std::byte>&& a_Data) { return Write(glm::uvec3(0), GetSize(), std::move(a_Data)); };
    std::vector<std::byte> Read(const glm::uvec3& a_Offset, const glm::uvec3& a_Size) const;
    std::vector<std::byte> Read() const { return Read(glm::uvec3(0), GetSize()); }
    void Map() const;
    void Unmap() const;

    /** @brief Allocates a new empty pixel buffer */
    void Allocate();
    /** @brief Fetches a color from the specified pixel, asserts that _data is not empty */
    PixelColor Load(const PixelCoord& a_TexCoord) const;
    /**
     * @brief Sets the pixel corresponding to texCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
     */
    void Store(
        const PixelCoord& a_TexCoord,
        const PixelColor& a_Color);

private:
    mutable ImageStorage _storage;
};

}
