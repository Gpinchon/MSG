/*
* @Author: gpinchon
* @Date:   2019-05-08 11:07:22
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-10 19:26:25
*/

#include "Texture/Texture2D.hpp" // for Texture2D
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Assets/Parser.hpp" // for TextureParser
#include "Parser/InternalTools.hpp" // for BTHeader, openFile
#include <glm/glm.hpp> // for glm::vec2
#include <memory> // for allocator, shared_ptr
#include <stdexcept> // for runtime_error
#include <stdio.h> // for fclose, fread, size_t
#include <string> // for operator+, char_traits, to_string

using namespace TabGraph;

void BTParse(const std::shared_ptr<Assets::Asset>&);

auto BTMimeExtension {
    Assets::Parser::AddMimeExtension("image/binary-terrain", ".bt")
};

auto BTMimesParsers {
    Assets::Parser::Add("image/binary-terrain", BTParse)
};

#pragma pack(1)
/// \private
struct BTHeader {
    unsigned char marker[10]; //"binterr1.3"
    int columns; //Width (east-west) dimension of the height grid.
    int rows; //Height (north-south) dimension of the height grid.
    short dataSize; //Bytes per elevation grid point, either 2 or 4.
    short fPoint; //Floating-point flag
        //If 1, the data consists of floating point values (float), otherwise they are signed integers.
    short hUnits; //Horizontal units
        //0: Degrees
        //1: Meters
        //2: Feet (international foot = .3048 meters)
        //3: Feet (U.S. survey foot = 1200/3937 meters)
    short UTMZone; //Indicates the UTM zone (1-60) if the file is in UTM.  Negative zone numbers are used for the southern hemisphere.
    short datum; //Indicates the Datum, see Datum Values below.
    double leftExtent; //The extents are specified in the coordinate space (projection) of the file.  For example, if the file is using UTM, then the extents are in UTM coordinates.
    double rightExtent;
    double bottomExtent;
    double topExtent;
    short projection; //External projection
        //0: Projection is fully described by this header
        //1: Projection is specified in a external .prj file
    float verticalScale; // (vertical units) Vertical units in meters, usually 1.0.  The value 0.0 should be interpreted as 1.0 to allow for backward compatibility.
    unsigned char padding[190]; //unused Bytes of value 0 are used to pad the rest of the header.
};
#pragma pack()

void BTParse(const std::shared_ptr<Assets::Asset> &asset)
{
    BTHeader header;
    Pixel::SizedFormat dataFormat;
    size_t readSize;
    auto uri{ asset->GetUri() };
    auto path = uri.DecodePath();
    auto fd = openFile(path.string());
    if ((readSize = fread(&header, 1, sizeof(BTHeader), fd) != sizeof(BTHeader))) {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path.string() + " : " + "Invalid file header, expected size " + std::to_string(sizeof(BTHeader)) + " got " + std::to_string(readSize));
    }
    if (header.dataSize == 2) {
        dataFormat = header.fPoint ? Pixel::SizedFormat::Float16_R : Pixel::SizedFormat::Int16_R;
    } else if (header.dataSize == 4) {
        dataFormat = header.fPoint ? Pixel::SizedFormat::Float32_R : Pixel::SizedFormat::Int32_R;
    } else {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path.string() + " : " + "Invalid data size " + std::to_string(header.dataSize));
    }
    size_t totalSize = header.dataSize * header.rows * header.columns;
    auto data = std::vector<std::byte>(totalSize);
    if ((readSize = fread(data.data(), 1, totalSize, fd) != totalSize)) {
        fclose(fd);
        throw std::runtime_error(std::string("[ERROR] ") + path.string() + " : " + "Invalid map size, expected size " + std::to_string(totalSize) + " got " + std::to_string(readSize));
    }
    fclose(fd);
    glm::ivec2 size(header.rows, header.columns);
    asset->SetAssetType(Assets::Image::AssetType);
    asset->assets.push_back(std::make_shared<Assets::Image>(size, dataFormat, data));
    asset->SetLoaded(true);
}
