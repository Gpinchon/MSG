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
enum class Compression {
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
#pragma pack(1)
struct t_bmp_pixel_24 {
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
        };
        unsigned bitSet : 24;
    };
};

struct t_bmp_pixel_32 {
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
        unsigned bitSet : 32;
    };
};

struct t_bmp_gamma {
    uint32_t red;
    uint32_t green;
    uint32_t blue;
};
struct t_bmp_cie_xyz {
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
};
struct t_bmp_cie_xyz_triple {
    t_bmp_cie_xyz red;
    t_bmp_cie_xyz green;
    t_bmp_cie_xyz blue;
};
struct t_bmp_color_mask {
    t_bmp_color_mask(
        uint8_t a_Red,
        uint8_t a_Green,
        uint8_t a_Blue,
        uint8_t a_Alpha)
        : red(a_Red)
        , green(a_Green)
        , blue(a_Blue)
        , alpha(a_Alpha)
    {
    }
    union {
        struct {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            uint8_t alpha;
        };
        uint32_t bitSet;
    };
};

/// \private
struct t_bmp_info {
    int32_t width                  = 0;
    int32_t height                 = 0;
    uint16_t color_planes          = 0;
    uint16_t bpp                   = 0;
    Compression compression_method = Compression::RGB;
    uint32_t size                  = 0;
    int32_t horizontal_resolution  = 0;
    int32_t vertical_resolution    = 0;
    uint32_t colors_used           = 0;
    uint32_t important_colors      = 0;
    t_bmp_color_mask red_mask      = { 1, 0, 0, 0 };
    t_bmp_color_mask green_mask    = { 0, 1, 0, 0 };
    t_bmp_color_mask blue_mask     = { 0, 0, 1, 0 };
    t_bmp_color_mask alpha_mask    = { 0, 0, 0, 1 };
    uint32_t color_space_type      = 0;
    t_bmp_cie_xyz_triple cie_xyz   = {};
    t_bmp_gamma gamma              = {};
    uint32_t intent                = 0;
    uint32_t profile_data          = 0;
    uint32_t profile_size          = 0;
    uint32_t reserved              = 0;
};

/// \private
struct t_bmp_header {
    uint16_t type        = 0x4D42;
    uint32_t size        = 0;
    uint16_t reserved1   = 0;
    uint16_t reserved2   = 0;
    uint32_t data_offset = 0;
};
#pragma pack()

/// \private
struct t_bmp_parser {
    FILE* fd                    = nullptr;
    t_bmp_info info             = {};
    t_bmp_header header         = {};
    std::vector<std::byte> data = {};
    unsigned size_read          = 0;
};

using namespace TabGraph;

static void prepare_header(t_bmp_header* header, t_bmp_info* info, std::shared_ptr<SG::Image> t)
{
    header->type                = 0x424D;
    header->size                = header->data_offset + (t->GetSize().x * t->GetSize().y * 4);
    info->width                 = t->GetSize().x;
    info->height                = t->GetSize().y;
    info->color_planes          = 1;
    info->bpp                   = 32; // t->GetPixelDescription().GetSize(); //t->bpp();
    info->size                  = t->GetSize().x * t->GetSize().y * 4;
    info->horizontal_resolution = 0x0ec4;
    info->vertical_resolution   = 0x0ec4;
}

void SaveBMP(std::shared_ptr<SG::Image> image, const std::string& imagepath)
{
    t_bmp_header header;
    t_bmp_info info;
    std::byte* padding = nullptr;
    int fd;

    prepare_header(&header, &info, image);
#ifdef _WIN32
    fd = open(imagepath.c_str(), O_RDWR | O_CREAT | O_BINARY);
#else
    fd = open(imagepath.c_str(), O_RDWR | O_CREAT | O_BINARY,
        S_IRWXU | S_IRWXG | S_IRWXO);
#endif
    if (write(fd, &header, sizeof(t_bmp_header)) != sizeof(t_bmp_header))
        throw std::runtime_error("Error while writing to " + imagepath);
    if (write(fd, &info, sizeof(t_bmp_info)) != sizeof(t_bmp_info))
        throw std::runtime_error("Error while writing to " + imagepath);
    for (auto y = 0u; y < image->GetSize().y; ++y) {
        for (auto x = 0u; x < image->GetSize().x; ++x) {
            auto floatColor { image->Load(SG::Pixel::Coord(x, y, 0)) };
            glm::vec<4, uint8_t> byteColor { glm::clamp(floatColor, 0.f, 1.f) };
            if (write(fd, &byteColor[0], 4) != 4)
                throw std::runtime_error("Error while writing to " + imagepath);
        }
    }
    // We should not need padding
    /*padding = new GLubyte[int(info.size - int64_t(t->Size().x * t->Size().y * t->bpp() / 8))]();
    write(fd, padding, info.size - (t->GetSize().x * t->GetSize().y * t->bpp() / 8));
    delete[] padding;*/
    close(fd);
}

static void convert_bmp_to_rgb(t_bmp_parser* parser)
{
    for (size_t i = 0; i < parser->data.size(); i += parser->info.bpp / 8) {
        if (parser->info.bpp == 24) {
            t_bmp_pixel_24* ptr  = reinterpret_cast<t_bmp_pixel_24*>(&parser->data.at(i));
            t_bmp_pixel_24 color = *ptr;
            ptr->red             = color.blue;
            ptr->green           = color.green;
            ptr->blue            = color.red;
        } else if (parser->info.bpp == 32) {
            t_bmp_pixel_32* ptr  = reinterpret_cast<t_bmp_pixel_32*>(&parser->data.at(i));
            t_bmp_pixel_32 color = *ptr;
            ptr->red             = color.alpha;
            ptr->green           = color.blue;
            ptr->blue            = color.green;
            ptr->alpha           = color.red;
        }
    }
}

static void convert_bitfield_bmp(t_bmp_parser* parser)
{
    auto redMask   = parser->info.red_mask.bitSet;
    auto greenMask = parser->info.green_mask.bitSet;
    auto blueMask  = parser->info.blue_mask.bitSet;
    auto alphaMask = parser->info.alpha_mask.bitSet;
    for (size_t i = 0; i < parser->data.size(); i += parser->info.bpp / 8) {
        if (parser->info.bpp == 24) {
            t_bmp_pixel_24* ptr  = reinterpret_cast<t_bmp_pixel_24*>(&parser->data.at(i));
            t_bmp_pixel_24 color = *ptr;
            auto red             = color.bitSet & redMask;
            auto green           = color.bitSet & greenMask;
            auto blue            = color.bitSet & blueMask;
            ptr->bitSet          = red | green | blue;
        } else if (parser->info.bpp == 32) {
            t_bmp_pixel_32* ptr  = reinterpret_cast<t_bmp_pixel_32*>(&parser->data.at(i));
            t_bmp_pixel_32 color = *ptr;
            auto red             = color.bitSet & redMask;
            auto green           = color.bitSet & greenMask;
            auto blue            = color.bitSet & blueMask;
            auto alpha           = color.bitSet & alphaMask;
            ptr->bitSet          = red | green | blue | alpha;
        }
    }
}

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
    if (p->info.compression_method == Compression::RGB)
        data_size = p->info.bpp / 8 * p->info.width * p->info.height;
    else
        data_size = p->info.size;
    fseek(p->fd, p->header.data_offset, SEEK_SET);
    p->data.resize(data_size);
    p->size_read = fread(p->data.data(), sizeof(std::byte), data_size, p->fd);
    fclose(p->fd);
    if (p->info.compression_method == Compression::BITFIELDS)
        convert_bitfield_bmp(p);
    convert_bmp_to_rgb(p);
    if (needsVerticalFlip)
        flip_bmp(p);
    return (0);
}

SG::Pixel::SizedFormat GetBMPPixelFormat(uint16_t bpp)
{
    switch (bpp) {
    case 8:
        return SG::Pixel::SizedFormat::Uint8_NormalizedR;
    case 24:
        return SG::Pixel::SizedFormat::Uint8_NormalizedRGB;
    case 32:
        return SG::Pixel::SizedFormat::Uint8_NormalizedRGBA;
    default:
        return SG::Pixel::SizedFormat::Unknown;
    }
}

std::shared_ptr<Asset> ParseBMP(const std::shared_ptr<Asset>& asset)
{
    t_bmp_parser parser;

    try {
        read_data(&parser, asset->GetUri().DecodePath());
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + asset->GetUri().DecodePath().string() + " : " + e.what());
    }
    auto buffer = std::make_shared<SG::Buffer>(parser.data);
    auto image  = std::make_shared<SG::Image2D>(
        GetBMPPixelFormat(parser.info.bpp),
        size_t(parser.info.width), size_t(parser.info.height),
        std::make_shared<SG::BufferView>(buffer, 0, buffer->size()));
    asset->AddObject(image);
    asset->SetLoaded(true);
    return asset;
}
}
