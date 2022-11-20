/*
* @Author: gpinchon
* @Date:   2021-02-03 17:50:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-03 17:50:55
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Uri.hpp>

#include <SG/Buffer/Buffer.hpp>

#include <fstream>

namespace TabGraph::Assets {
std::shared_ptr<Asset> ParseBinaryData(const std::shared_ptr<Asset>& asset)
{
    std::shared_ptr<SG::Buffer> binaryData;
    {
        auto& uri{ asset->GetUri() };
        if (uri.GetScheme() == "data") {
            binaryData = std::make_shared<SG::Buffer>(DataUri(uri).Decode());
        }
        else {
            const auto path{ uri.DecodePath() };
            const auto size{ std::filesystem::file_size(path) };
            binaryData = std::make_shared<SG::Buffer>(size);
            std::basic_fstream<std::byte> file(path, std::ios::binary);
            file.read(binaryData->data(), size);
        }
    }
    asset->assets.push_back(binaryData);
    asset->SetAssetType("BinaryData");
    asset->SetLoaded(true);
    return asset;
}
}
