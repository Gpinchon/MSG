/*
* @Author: gpinchon
* @Date:   2019-04-04 13:53:19
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:46:05
*/

#include <Assets/Image.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Buffer/View.hpp>
#include <Config.hpp>
#include <Texture/PixelUtils.hpp>
#include <Debug.hpp>

#include <memory>
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_rwops.h>
#include <SDL_surface.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <glm/common.hpp>

using namespace TabGraph::Assets;

void SDL2ImageParser(const std::shared_ptr<Asset>&);

auto SDL2MimesExtensions = {
    Parser::AddMimeExtension("image/tga", ".tga"),
    Parser::AddMimeExtension("image/bmp", ".bmp"),
    Parser::AddMimeExtension("image/x-portable-anymap", ".pnm"),
    Parser::AddMimeExtension("image/x-portable-graymap", ".pgm"),
    Parser::AddMimeExtension("image/x-portable-pixmap", ".ppm"),
    Parser::AddMimeExtension("image/x-xpixmap", ".xpm"),
    Parser::AddMimeExtension("image/xcf", ".xcf"),
    Parser::AddMimeExtension("image/x-pcx", ".pcx"),
    Parser::AddMimeExtension("image/gif", ".gif"),
    Parser::AddMimeExtension("image/jpeg", ".jpg"),
    Parser::AddMimeExtension("image/jpeg", ".jpeg"),
    Parser::AddMimeExtension("image/tiff", ".tif"),
    Parser::AddMimeExtension("image/tiff", ".tiff"),
    Parser::AddMimeExtension("image/x-ilbm", ".lbm"),
    Parser::AddMimeExtension("image/x-ilbm", ".iff"),
    Parser::AddMimeExtension("image/png", ".png")
};

auto SDL2MimesParsers = {
    Parser::Add("image/tga", SDL2ImageParser),
    Parser::Add("image/bmp", SDL2ImageParser),
    Parser::Add("image/x-portable-anymap", SDL2ImageParser),
    Parser::Add("image/x-portable-graymap", SDL2ImageParser),
    Parser::Add("image/x-portable-pixmap", SDL2ImageParser),
    Parser::Add("image/x-xpixmap", SDL2ImageParser),
    Parser::Add("image/xcf", SDL2ImageParser),
    Parser::Add("image/x-pcx", SDL2ImageParser),
    Parser::Add("image/gif", SDL2ImageParser),
    Parser::Add("image/jpeg", SDL2ImageParser),
    Parser::Add("image/jpeg", SDL2ImageParser),
    Parser::Add("image/tiff", SDL2ImageParser),
    Parser::Add("image/tiff", SDL2ImageParser),
    Parser::Add("image/x-ilbm", SDL2ImageParser),
    Parser::Add("image/x-ilbm", SDL2ImageParser),
    Parser::Add("image/png", SDL2ImageParser)
};

std::string hexToString(int hex)
{
    std::ostringstream stream;
    stream << std::hex << hex;
    return stream.str();
}

void SDL2ImageParser(const std::shared_ptr<Asset>& container)
{
    auto uri{ container->GetUri() };
    SDL_RWops* rwOps{ nullptr };
    std::vector<std::byte> data;
    if (uri.GetScheme() == "data") {
        data = DataUri(uri).Decode();
        auto buffers = container->Get<TabGraph::Buffer::View>();
        if (data.empty() && !buffers.empty()) {
            auto& bufferView{ buffers.at(0) };
            if (bufferView != nullptr) { //We're loading raw bytes from a Buffer::View
                bufferView->Load();
                data = std::vector<std::byte>(bufferView->begin(), bufferView->end());
            }
        }
        rwOps = SDL_RWFromMem(data.data(), data.size());
    }
    else {
        rwOps = SDL_RWFromFile(uri.DecodePath().u8string().c_str(), "rb");
    }

    auto surface = IMG_Load_RW(rwOps, 1);
    assert(surface != nullptr);
    assert(surface->format != nullptr);

    debugLog("Image Format before conversion :");
    debugLog(SDL_GetPixelFormatName(surface->format->format));
    debugLog(int(surface->format->BitsPerPixel));
    debugLog(int(surface->format->BytesPerPixel));
    debugLog(hexToString(surface->format->Rmask));
    debugLog(hexToString(surface->format->Gmask));
    debugLog(hexToString(surface->format->Bmask));
    debugLog(hexToString(surface->format->Amask));

    if (SDL_PIXELFORMAT_RGBA32 != surface->format->format) {
        auto newSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        assert(newSurface != nullptr);
        SDL_FreeSurface(surface);
        surface = newSurface;
    }
    const auto bufferSize{ surface->h * surface->pitch };
    const auto pixelFormat = TabGraph::Pixel::SizedFormat::Uint8_NormalizedRGBA;
    auto image = std::make_shared<Image>(glm::ivec2(surface->w, surface->h), pixelFormat, std::vector((std::byte*)surface->pixels, (std::byte*)surface->pixels + bufferSize));
    SDL_FreeSurface(surface);
    surface = nullptr;
    if (container->parsingOptions.image.maximumResolution != -1) {
        auto texRes{ glm::min(image->GetSize(), glm::ivec2(container->parsingOptions.image.maximumResolution)) };
        image->SetSize(texRes, Image::SamplingFilter::Bilinear);
    }
    container->assets.push_back(image);
    container->SetAssetType(Image::AssetType);
    container->SetLoaded(true);
}
