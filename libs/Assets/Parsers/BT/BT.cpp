#include <MSG/Assets/Asset.hpp>
#include <MSG/Image.hpp>

#include <glm/glm.hpp> // for glm::vec2

#include <fstream>
#include <memory> // for allocator, shared_ptr
#include <stdexcept> // for runtime_error
#include <string> // for operator+, char_traits, to_string

using namespace MSG;

namespace MSG::Assets {
#pragma pack(1)
/// \private
struct BTHeader {
    unsigned char marker[10]; //"binterr1.3"
    int columns; // Width (east-west) dimension of the height grid.
    int rows; // Height (north-south) dimension of the height grid.
    short dataSize; // Bytes per elevation grid point, either 2 or 4.
    short fPoint; // Floating-point flag
                  // If 1, the data consists of floating point values (float), otherwise they are signed integers.
    short hUnits; // Horizontal units
                  // 0: Degrees
                  // 1: Meters
                  // 2: Feet (international foot = .3048 meters)
                  // 3: Feet (U.S. survey foot = 1200/3937 meters)
    short UTMZone; // Indicates the UTM zone (1-60) if the file is in UTM.  Negative zone numbers are used for the southern hemisphere.
    short datum; // Indicates the Datum, see Datum Values below.
    double leftExtent; // The extents are specified in the coordinate space (projection) of the file.  For example, if the file is using UTM, then the extents are in UTM coordinates.
    double rightExtent;
    double bottomExtent;
    double topExtent;
    short projection; // External projection
                      // 0: Projection is fully described by this header
                      // 1: Projection is specified in a external .prj file
    float verticalScale; // (vertical units) Vertical units in meters, usually 1.0.  The value 0.0 should be interpreted as 1.0 to allow for backward compatibility.
    unsigned char padding[190]; // unused Bytes of value 0 are used to pad the rest of the header.
};
#pragma pack()

std::shared_ptr<Assets::Asset> ParseBT(const std::shared_ptr<Assets::Asset>& asset)
{
    BTHeader header {};
    PixelSizedFormat dataFormat;
    auto path = asset->GetUri().DecodePath();
    auto file = std::basic_fstream<std::byte>(path.string(), std::ios::binary);
    try {
        file.read((std::byte*)&header, sizeof(BTHeader));
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("file:[") + path.string() + "]\nError while reading file header : " + e.what());
        return asset;
    }
    if (header.dataSize == 2) {
        dataFormat = header.fPoint ? PixelSizedFormat::Float16_R : PixelSizedFormat::Int16_R;
    } else if (header.dataSize == 4) {
        dataFormat = header.fPoint ? PixelSizedFormat::Float32_R : PixelSizedFormat::Int32_R;
    } else {
        throw std::runtime_error(std::string("file:[") + path.string() + "]\nInvalid Data size : " + std::to_string(header.dataSize));
        return asset;
    }
    const auto totalSize = header.dataSize * header.rows * header.columns;
    auto data            = std::vector<std::byte>(totalSize);
    try {
        file.read(data.data(), totalSize);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("file:[") + path.string() + "]\nError while reading file data : " + e.what());
        return asset;
    }
    auto image = std::make_shared<Image>(
        ImageInfo {
            .width     = uint32_t(header.rows),
            .height    = uint32_t(header.columns),
            .pixelDesc = dataFormat,
            .storage   = ImageStorage(std::move(data)),
        });
    asset->SetAssetType("image/binary-terrain");
    asset->AddObject(image);
    asset->SetLoaded(true);
    return asset;
}
}
