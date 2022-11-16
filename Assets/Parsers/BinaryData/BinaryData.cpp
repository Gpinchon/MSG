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
        auto uri{ asset->GetUri() };
        std::vector<std::byte> data;
        if (uri.GetScheme() == "data") {
            data = DataUri(uri).Decode();
        }
        else {
            auto path{ uri.DecodePath() };
            auto size{ std::filesystem::file_size(path) };
            data.resize(size);
            std::ifstream file(path, std::ios::binary);
            file.read(reinterpret_cast<char*>(data.data()), data.size());
        }
        binaryData = std::make_shared<SG::Buffer>(data);
    }
    asset->assets.push_back(binaryData);
    asset->SetAssetType("BinaryData");
    asset->SetLoaded(true);
    return asset;
}
}
