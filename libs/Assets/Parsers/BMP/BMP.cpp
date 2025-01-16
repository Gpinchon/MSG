/*
 * @Author: gpinchon
 * @Date:   2019-02-22 16:13:28
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-01-11 08:46:17
 */

#include <Assets/Asset.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>

#include <glm/glm.hpp> // for s_vec2, glm::vec2

#include <algorithm>
#include <errno.h> // for errno
#include <fcntl.h> // for O_BINARY, O_CREAT, O_RDWR
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fread, fopen, fseek, SEE...
#include <string.h> // for memset, strerror
#include <sys/stat.h> // for S_IRWXG, S_IRWXO, S_IRWXU
#ifndef _MSC_VER
#include <unistd.h>
#endif //_MSC_VER

#ifdef _WIN32
#include <io.h>
#ifndef R_OK
#define R_OK 4
#endif
#else
#include <sys/io.h>
#endif // for write, access, close, open, R_OK

#ifndef O_BINARY
#define O_BINARY 0x0004
#endif

namespace TabGraph::Assets {
#pragma pack(push, 1)
/// \private
enum class e_bmp_compression : uint32_t {
    RGB       = 0x0000,
    RLE8      = 0x0001,
    RLE4      = 0x0002,
    BITFIELDS = 0x0003,
    JPEG      = 0x0004,
    PNG       = 0x0005,
    CMYK      = 0x000B,
    CMYKRLE8  = 0x000C,
    CMYKRLE4  = 0x000D
};

/// \private
/// use curiously recurring template pattern for correct memcpy size
template <typename PixelType>
struct t_bmp_pixel {
    uint32_t Bitset() const
    {
        uint32_t bitset = 0;
        std::memcpy(&bitset, this, sizeof(PixelType));
        return bitset;
    }
    void operator=(const uint32_t& a_Val)
    {
        std::memcpy(this, &a_Val, sizeof(PixelType));
    }
};

/// \private
struct t_bmp_pixel_16 : t_bmp_pixel<t_bmp_pixel_16> {
    using t_bmp_pixel::operator=;
    uint8_t red : 4;
    uint8_t green : 4;
    uint8_t blue : 4;
};
static_assert(sizeof(t_bmp_pixel_16) == 2);

/// \private
struct t_bmp_pixel_24 : t_bmp_pixel<t_bmp_pixel_24> {
    using t_bmp_pixel::operator=;
    uint8_t red : 8;
    uint8_t green : 8;
    uint8_t blue : 8;
};
static_assert(sizeof(t_bmp_pixel_24) == 3);

/// \private
struct t_bmp_pixel_32 : t_bmp_pixel<t_bmp_pixel_32> {
    using t_bmp_pixel::operator=;
    uint8_t red : 8;
    uint8_t green : 8;
    uint8_t blue : 8;
    uint8_t alpha : 8;
};
static_assert(sizeof(t_bmp_pixel_32) == 4);

/// \private
struct t_bmp_gamma {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
};

/// \private
struct t_bmp_cie_xyz {
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
};

/// \private
struct t_bmp_cie_xyz_triple {
    t_bmp_cie_xyz red;
    t_bmp_cie_xyz green;
    t_bmp_cie_xyz blue;
};

/// \private
struct t_bmp_color_mask : t_bmp_pixel_32 {
    t_bmp_color_mask(
        uint8_t a_Red,
        uint8_t a_Green,
        uint8_t a_Blue,
        uint8_t a_Alpha)
    {
        red   = a_Red;
        green = a_Green;
        blue  = a_Blue;
        alpha = a_Alpha;
    }
};

/// \private
struct t_bmp_info {
    int32_t width                        = 0;
    int32_t height                       = 0;
    uint16_t color_planes                = 0;
    uint16_t bpp                         = 0;
    e_bmp_compression compression_method = e_bmp_compression::RGB;
    uint32_t size                        = 0;
    int32_t horizontal_resolution        = 0;
    int32_t vertical_resolution          = 0;
    uint32_t colors_used                 = 0;
    uint32_t important_colors            = 0;
    t_bmp_color_mask red_mask            = { 1, 0, 0, 0 };
    t_bmp_color_mask green_mask          = { 0, 1, 0, 0 };
    t_bmp_color_mask blue_mask           = { 0, 0, 1, 0 };
    t_bmp_color_mask alpha_mask          = { 0, 0, 0, 1 };
    uint32_t color_space_type            = 0;
    t_bmp_cie_xyz_triple cie_xyz         = {};
    t_bmp_gamma gamma                    = {};
    uint32_t intent                      = 0;
    uint32_t profile_data                = 0;
    uint32_t profile_size                = 0;
    uint32_t reserved                    = 0;
};

/// \private
struct t_bmp_header {
    uint16_t type        = 0x4D42;
    uint32_t size        = 0;
    uint16_t reserved1   = 0;
    uint16_t reserved2   = 0;
    uint32_t data_offset = 0;
};
#pragma pack(pop)

/// \private
struct t_bmp_parser {
    FILE* fd                    = nullptr;
    t_bmp_info info             = {};
    t_bmp_header header         = {};
    std::vector<std::byte> data = {};
};

static void flip_bmp(t_bmp_parser* parser)
{
    std::array<std::byte, 4096> pixel_temp;
    auto lineSize = parser->info.width * (parser->info.bpp / 8);
    for (auto y = 0, y1 = parser->info.height - 1; y < parser->info.height / 2; y++, y1--) {
        auto topLineIndex    = y * lineSize;
        auto bottomLineIndex = y1 * lineSize;
        std::swap_ranges(
            &parser->data.at(topLineIndex), &parser->data.at(topLineIndex + lineSize),
            &parser->data.at(bottomLineIndex));
    }
}

static int read_data(t_bmp_parser* p, const std::filesystem::path& path)
{
    unsigned data_size;
    bool needsVerticalFlip = true;

    if (access(path.string().c_str(), R_OK) != 0) {
        throw std::runtime_error(strerror(errno));
    }
    if ((p->fd = fopen(path.string().c_str(), "rb")) == nullptr) {
        throw std::runtime_error(strerror(errno));
    }
    auto readReturn = fread(&p->header, 1, sizeof(p->header), p->fd);
    if (readReturn != sizeof(p->header) || p->header.type != 0x4D42) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Header");
    }
    uint32_t bmp_info_size = 0;
    readReturn             = fread(&bmp_info_size, 1, sizeof(bmp_info_size), p->fd);
    if (readReturn != sizeof(bmp_info_size)) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Info");
    }
    readReturn = fread(&p->info, 1, bmp_info_size, p->fd);
    if (readReturn != bmp_info_size) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Info");
    }
    if (p->info.height < 0) {
        needsVerticalFlip = false;
        p->info.height    = std::abs(p->info.height);
    }
    if (p->info.compression_method == e_bmp_compression::RGB)
        data_size = p->info.bpp / 8 * p->info.width * p->info.height;
    else
        data_size = p->info.size;
    fseek(p->fd, p->header.data_offset, SEEK_SET);
    p->data.resize(data_size);
    fread(p->data.data(), sizeof(std::byte), data_size, p->fd);
    fclose(p->fd);
    if (needsVerticalFlip)
        flip_bmp(p);
    return (0);
}

static auto UnpackBitfield16(const t_bmp_parser& parser)
{
    auto redMask   = parser.info.red_mask.Bitset();
    auto greenMask = parser.info.green_mask.Bitset();
    auto blueMask  = parser.info.blue_mask.Bitset();
    auto alphaMask = parser.info.alpha_mask.Bitset();
    auto res       = std::make_shared<SG::Image2D>(SG::Pixel::SizedFormat::Uint8_NormalizedRGB, parser.info.width, parser.info.height);
    res->Allocate();
    SG::TypedBufferAccessor<t_bmp_pixel_24> accessor(res->GetBufferAccessor());
    for (size_t i = 0; i < parser.data.size() / sizeof(t_bmp_pixel_16); i++) {
        t_bmp_pixel_24 color = {};
        auto inIndex         = i * sizeof(t_bmp_pixel_16);
        auto inPtr           = reinterpret_cast<const t_bmp_pixel_16*>(&parser.data.at(inIndex));
        auto red             = inPtr->Bitset() & redMask;
        auto green           = inPtr->Bitset() & greenMask;
        auto blue            = inPtr->Bitset() & blueMask;
        color                = red | green | blue;
        accessor.at(i).red   = color.red;
        accessor.at(i).green = color.green;
        accessor.at(i).blue  = color.blue;
    }
    return res;
}

static auto UnpackBitfield32(const t_bmp_parser& parser)
{
    auto redMask   = parser.info.red_mask.Bitset();
    auto greenMask = parser.info.green_mask.Bitset();
    auto blueMask  = parser.info.blue_mask.Bitset();
    auto alphaMask = parser.info.alpha_mask.Bitset();
    auto res       = std::make_shared<SG::Image2D>(SG::Pixel::SizedFormat::Uint8_NormalizedRGBA, parser.info.width, parser.info.height);
    res->Allocate();
    SG::TypedBufferAccessor<t_bmp_pixel_32> accessor(res->GetBufferAccessor());
    for (size_t i = 0; i < parser.data.size() / sizeof(t_bmp_pixel_32); i++) {
        t_bmp_pixel_32 color = {};
        auto inIndex         = i * sizeof(t_bmp_pixel_32);
        auto inPtr           = reinterpret_cast<const t_bmp_pixel_32*>(&parser.data.at(inIndex));
        auto red             = inPtr->Bitset() & redMask;
        auto green           = inPtr->Bitset() & greenMask;
        auto blue            = inPtr->Bitset() & blueMask;
        auto alpha           = inPtr->Bitset() & alphaMask;
        color                = red | green | blue | alpha;
        accessor.at(i).red   = color.red;
        accessor.at(i).green = color.green;
        accessor.at(i).blue  = color.blue;
        accessor.at(i).alpha = color.alpha;
    }
    return res;
}

static std::shared_ptr<SG::Image2D> UnpackBitfield(const t_bmp_parser& a_Parser)
{
    auto bpp = a_Parser.info.bpp;
    assert(bpp == 16 || bpp == 32); // Bitfield is correct only for those BPP
    if (bpp == 16) {
        return UnpackBitfield16(a_Parser);
    } else if (bpp == 32) {
        return UnpackBitfield32(a_Parser);
    }
    return nullptr;
}

static void ConvertToRGB(SG::Image2D& a_Image)
{
    if (a_Image.GetPixelDescription().GetSizedFormat() == SG::Pixel::SizedFormat::Uint8_NormalizedRGB) {
        SG::TypedBufferAccessor<t_bmp_pixel_24> accessor(a_Image.GetBufferAccessor());
        for (auto& pixel : accessor) {
            t_bmp_pixel_24 color = pixel;
            pixel.red            = color.blue;
            pixel.green          = color.green;
            pixel.blue           = color.red;
        }

    } else if (a_Image.GetPixelDescription().GetSizedFormat() == SG::Pixel::SizedFormat::Uint8_NormalizedRGBA) {
        SG::TypedBufferAccessor<t_bmp_pixel_32> accessor(a_Image.GetBufferAccessor());
        for (auto& pixel : accessor) {
            t_bmp_pixel_32 color = pixel;
            pixel.red            = color.alpha;
            pixel.green          = color.blue;
            pixel.blue           = color.green;
            pixel.alpha          = color.red;
        }
    }
}

/// Convert data to something that's understandable for the engine
static auto ConvertData(const t_bmp_parser& a_Parser)
{
    std::shared_ptr<SG::Image2D> res;
    if (a_Parser.info.compression_method == e_bmp_compression::RGB) {
        // copy as is
        auto buffer = std::make_shared<SG::Buffer>(a_Parser.data);
        res         = std::make_shared<SG::Image2D>(
            SG::Pixel::SizedFormat::Uint8_NormalizedRGB,
            size_t(a_Parser.info.width), size_t(a_Parser.info.height),
            std::make_shared<SG::BufferView>(buffer, 0, buffer->size()));
    } else if (a_Parser.info.compression_method == e_bmp_compression::BITFIELDS)
        res = UnpackBitfield(a_Parser); // time for some bit-twiddling
    ConvertToRGB(*res);
    return res;
}

std::shared_ptr<Asset> ParseBMP(const std::shared_ptr<Asset>& asset)
{
    t_bmp_parser parser;

    try {
        read_data(&parser, asset->GetUri().DecodePath());
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + asset->GetUri().DecodePath().string() + " : " + e.what());
    }
    auto image           = ConvertData(parser);
    glm::uvec2 imageSize = image->GetSize();
    glm::uvec2 maxSize   = {
        asset->parsingOptions.image.maxWidth,
        asset->parsingOptions.image.maxHeight
    };
    if (glm::any(glm::greaterThan(imageSize, maxSize))) {
        auto newImageSize = glm::min(imageSize, maxSize);
        auto newImage     = std::make_shared<SG::Image2D>(image->GetPixelDescription(), newImageSize.x, newImageSize.y);
        newImage->Allocate();
        image->Blit(*newImage, { 0u, 0u, 0u }, image->GetSize(), SG::ImageFilter::Bilinear);
        image = newImage;
    }
    asset->AddObject(image);
    asset->SetLoaded(true);
    return asset;
}
}
