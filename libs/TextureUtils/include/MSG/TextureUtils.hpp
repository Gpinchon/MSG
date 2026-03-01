#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class Texture;
struct PixelDescriptor;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
/**
 * @brief automatically generate mipmaps.
 * Base level has to be set.
 * The nbr of mipmaps is computed with : floor(log2(max(size.x, size.y[, size.z])))
 * It is recommended to generate mipmaps BEFORE compressing the texture
 */
void TextureGenerateMipmaps(Texture& a_Dst);
/**
 * @brief replaces the stored Images by compressed versions
 */
void TextureCompress(Texture& a_Dst);
/**
 * @brief replaces the stored Images by decompressed versions
 */
void TextureDecompress(Texture& a_Dst);
/**
 * @brief converts the source to the specified pixel format
 */
Texture TextureConvert(const Texture& a_Src, const PixelDescriptor& a_PixelDesc);
/**
 * @brief resizes the source texture to the specified size
 */
Texture TextureResize(const Texture& a_Src, const glm::uvec3& a_NewSize);
}
