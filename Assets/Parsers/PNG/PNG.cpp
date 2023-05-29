#include <Assets/Asset.hpp>

#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>

#include <Tools/Debug.hpp>

#include <png.h>

#include <algorithm>
#include <fstream>
#include <iostream>

namespace TabGraph::Assets {
namespace PNG {
    struct Struct {
        Struct(png_structp a_Ptr)
            : _ptr(a_Ptr)
        {
        }
        operator png_structp() const { return _ptr; }
        operator bool() const { return _ptr != nullptr; }

    protected:
        png_structp _ptr;
    };
    struct Read : Struct {
        Read()
            : Struct(png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))
        {
        }
        ~Read() { png_destroy_read_struct(&_ptr, nullptr, nullptr); }
    };
    struct Write : Struct {
        Write()
            : Struct(png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))
        {
        }
        ~Write() { png_destroy_write_struct(&_ptr, nullptr); }
    };
    struct Info {
        Info(const Struct& a_PngRead)
            : _read(a_PngRead)
            , _ptr(png_create_info_struct(_read))
        {
        }
        ~Info() { png_destroy_info_struct(_read, &_ptr); }
        operator png_infop() const { return _ptr; }
        operator bool() const { return _ptr != nullptr; }

    private:
        const Struct& _read;
        png_infop _ptr;
    };
    void ReadData(png_structp pngPtr, png_bytep data, png_size_t length)
    {
        // Here we get our IO pointer back from the read struct.
        // This is the parameter we passed to the png_set_read_fn() function.
        // Our std::istream pointer.
        png_voidp a = png_get_io_ptr(pngPtr);
        // Cast the pointer to std::istream* and read 'length' bytes into 'data'
        ((std::istream*)a)->read((char*)data, length);
    }
    constexpr auto SIGSIZE = 8;
    bool Validate(std::istream& source)
    {
        png_byte header[SIGSIZE] {};
        source.read((char*)header, SIGSIZE);
        if (png_sig_cmp(header, 0, SIGSIZE)) {
            errorLog("png_sig_cmp failed");
            return false;
        }

        return true;
    }

    auto GetSizedFormat(uint8_t a_Channels, uint8_t a_BitDepth)
    {
        SG::Pixel::UnsizedFormat format = SG::Pixel::UnsizedFormat::Unknown;
        switch (a_Channels) {
        case 1:
            format = SG::Pixel::UnsizedFormat::R;
            break;
        case 2:
            format = SG::Pixel::UnsizedFormat::RG;
            break;
        case 3:
            format = SG::Pixel::UnsizedFormat::RGB;
            break;
        case 4:
            format = SG::Pixel::UnsizedFormat::RGBA;
            break;
        }
        SG::Pixel::Type type = SG::Pixel::Type::Unknown;
        switch (a_BitDepth) {
        case 8:
            type = SG::Pixel::Type::Uint8;
            break;
        case 16:
            type = SG::Pixel::Type::Uint16;
            break;
        }
        return SG::Pixel::GetSizedFormat(format, type, true);
    }
}

std::shared_ptr<Asset> ParsePNG(const std::shared_ptr<Asset>& a_Container)
{
    auto path = a_Container->GetUri().DecodePath();
    std::ifstream file(path, std::ios_base::binary);
    if (!PNG::Validate(file))
        return a_Container;
    PNG::Read pngRead;
    if (!pngRead) {
        debugLog("png_create_read_struct failed");
        return a_Container;
    }
    PNG::Info pngInfo(pngRead);
    if (!pngInfo) {
        debugLog("png_create_info_struct failed");
        return a_Container;
    }
    if (setjmp(png_jmpbuf(pngRead))) {
        debugLog("Error while reading PNG");
        return a_Container;
    }
    png_set_read_fn(pngRead, (png_voidp)&file, PNG::ReadData);
    png_set_sig_bytes(pngRead, PNG::SIGSIZE);
    png_read_info(pngRead, pngInfo); // Now call png_read_info with our pngPtr as image handle, and infoPtr to receive the file info.
    auto width = png_get_image_width(pngRead, pngInfo);
    auto height = png_get_image_height(pngRead, pngInfo);
    auto bitDepth = png_get_bit_depth(pngRead, pngInfo);
    auto channels = png_get_channels(pngRead, pngInfo);
    auto colorT = png_get_color_type(pngRead, pngInfo);
    png_set_expand(pngRead);
    bitDepth = std::max(bitDepth, uint8_t(8));

    /*if (png_get_valid(pngRead, pngInfo, PNG_INFO_tRNS)) {
            png_set_expand(pngRead);
            channels += 1;
    }*/
    if (bitDepth == 16)
        png_set_strip_16(pngRead);
    png_read_update_info(pngRead, pngInfo);
    std::vector<std::byte*> rows(height, nullptr);
    auto data = std::make_shared<SG::Buffer>(width * height * bitDepth * channels / 8);
    const auto stride = width * bitDepth * channels / 8;
    for (size_t i = 0; i < height; i++) {
        auto q = (height - i - 1) * stride;
        rows.at(i) = &data->at(q);
    }
    png_read_image(pngRead, (png_bytepp)rows.data());

    auto image = std::make_shared<SG::Image>();
    image->SetType(SG::Image::Type::Image2D);
    image->SetPixelDescription(PNG::GetSizedFormat(channels, bitDepth));
    image->SetSize({ width, height, 1 });
    image->SetBufferView(std::make_shared<SG::BufferView>(data, 0, data->size()));
    a_Container->AddObject(image);
    a_Container->SetLoaded(true);
    return a_Container;
}
}
