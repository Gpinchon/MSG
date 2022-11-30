/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:17
*/

#include <Assets/Asset.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>

#include <glm/glm.hpp> // for s_vec2, glm::vec2
#include <errno.h> // for errno
#include <fcntl.h> // for O_BINARY, O_CREAT, O_RDWR
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fread, fopen, fseek, SEE...
#include <string.h> // for memset, strerror
#include <sys/stat.h> // for S_IRWXG, S_IRWXO, S_IRWXU

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
#pragma pack(1)
/// \private
struct t_bmp_info {
    uint32_t    header_size{ sizeof(t_bmp_info) };
    int32_t     width{ 0 };
    int32_t     height{ 0 };
    uint16_t    color_planes{ 0 };
    uint16_t    bpp{ 0 };
    uint32_t    compression_method{ 0 };
    uint32_t    size{ 0 };
    int32_t     horizontal_resolution{ 0 };
    int32_t     vertical_resolution{ 0 };
    uint32_t    totalColors{ 0 };
    uint32_t    important_colors{ 0 };
};

/// \private
struct t_bmp_header {
    std::byte    type[2]{ std::byte(0), std::byte(0) };
    uint32_t     size{ 0 };
    std::byte    reserved1[2]{ std::byte(0), std::byte(0) };
    std::byte    reserved2[2]{ std::byte(0), std::byte(0) };
    uint32_t     data_offset{ sizeof(t_bmp_header) + sizeof(t_bmp_info) };
};
#pragma pack()

/// \private
struct t_bmp_parser {
    FILE* fd{ nullptr };
    t_bmp_info info;
    t_bmp_header header;
    std::shared_ptr<SG::Buffer> data;
    unsigned size_read{ 0 };
};

using namespace TabGraph;

static void prepare_header(t_bmp_header* header, t_bmp_info* info, std::shared_ptr<SG::Image> t)
{
    header->type[0] = std::byte(0x42);
    header->type[1] = std::byte(0x4D);
    header->size = header->data_offset + (t->GetSize().x * t->GetSize().y * 4);
    info->width = t->GetSize().x;
    info->height = t->GetSize().y;
    info->color_planes = 1;
    info->bpp = 32;//t->GetPixelDescription().GetSize(); //t->bpp();
    info->size = t->GetSize().x * t->GetSize().y * 4;
    info->horizontal_resolution = 0x0ec4;
    info->vertical_resolution = 0x0ec4;
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
            auto floatColor{ image->GetColor(SG::Pixel::Coord(x, y, 0)) };
            glm::vec<4, uint8_t> byteColor{ glm::clamp(floatColor, 0.f, 1.f) };
            if (write(fd, &byteColor[0], 4) != 4)
                throw std::runtime_error("Error while writing to " + imagepath);
        }
    }
    //We should not need padding
    /*padding = new GLubyte[int(info.size - int64_t(t->Size().x * t->Size().y * t->bpp() / 8))]();
    write(fd, padding, info.size - (t->GetSize().x * t->GetSize().y * t->bpp() / 8));
    delete[] padding;*/
    close(fd);
}

static void convert_bmp(t_bmp_parser* parser)
{
    std::vector<std::byte> pixel_temp{ parser->data->size() };
    std::byte rgba[4]{ std::byte(0), std::byte(0), std::byte(0), std::byte(0) };
    int i[3]{ 0, -1, 0 };

    while (++i[1] < parser->info.width) {
        i[2] = -1;
        while (++i[2] < parser->info.height) {
            rgba[0] = parser->data->at(i[0] + 1);
            rgba[1] = parser->data->at(i[0] + 2);
            rgba[2] = parser->data->at(i[0] + 3);
            rgba[3] = parser->data->at(i[0] + 0);
            pixel_temp[i[0] + 0] = rgba[0];
            pixel_temp[i[0] + 1] = rgba[1];
            pixel_temp[i[0] + 2] = rgba[2];
            pixel_temp[i[0] + 3] = rgba[3];
            i[0] += (parser->info.bpp / 8);
        }
    }
    parser->data->SetRawData(pixel_temp);
}

static int read_data(t_bmp_parser* p, const std::filesystem::path& path)
{
    unsigned data_size;

    if (access(path.string().c_str(), R_OK) != 0) {
        throw std::runtime_error(strerror(errno));
    }
    if ((p->fd = fopen(path.string().c_str(), "rb")) == nullptr) {
        throw std::runtime_error(strerror(errno));
    }
    auto readReturn = fread(&p->header, 1, sizeof(p->header), p->fd);
    if (readReturn != sizeof(p->header) || p->header.type[0] != std::byte(0x42) || p->header.type[1] != std::byte(0x4D)) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Header");
    }
    readReturn = fread(&p->info, 1, sizeof(p->info), p->fd);
    if (readReturn != sizeof(p->info)) {
        fclose(p->fd);
        throw std::runtime_error("Wrong Info");
    }
    if (p->info.width * p->info.bpp / 8 % 2)
        data_size = 4 * p->info.width * p->info.height;
    else
        data_size = p->info.bpp / 8 * p->info.width * p->info.height;
    fseek(p->fd, p->header.data_offset, SEEK_SET);
    p->data->resize(data_size);
    p->size_read = fread(p->data->data(), sizeof(std::byte), data_size, p->fd);
    fclose(p->fd);
    if (p->info.bpp == 32) {
        convert_bmp(p);
    }
    return (0);
}

SG::Pixel::SizedFormat GetBMPPixelFormat(uint16_t bpp) {
    switch (bpp)
    {
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
    auto size{ glm::ivec3(parser.info.width, parser.info.height, 1) };
    auto format{ GetBMPPixelFormat(parser.info.bpp) };
    auto image{ std::make_shared<SG::Image>() };
    image->SetType(SG::Image::Type::Image2D);
    image->SetSize(size);
    image->SetPixelDescription({ format });
    image->SetBufferView(std::make_shared<SG::BufferView>(parser.data, 0, parser.data->size()));
    asset->Add(image);
    asset->SetLoaded(true);
    return asset;
}

}
