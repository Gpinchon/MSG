/*
* @Author: gpinchon
* @Date:   2021-02-03 17:50:02
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-03 17:50:55
*/

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/BinaryData.hpp>
#include <Assets/Uri.hpp>

#include <fstream>

using namespace TabGraph;

void ParseBinaryData(const std::shared_ptr<Assets::Asset>&);

auto bufferMime { Assets::Parser::AddMimeExtension("application/octet-stream", ".bin") };
auto bufferParser { Assets::Parser::Add("application/octet-stream", ParseBinaryData) };

void ParseBinaryData(const std::shared_ptr<Assets::Asset>& asset)
{
    std::shared_ptr<Assets::BinaryData> binaryData;
    {
        auto uri { asset->GetUri() };
        std::vector<std::byte> data;
        if (uri.GetScheme() == "data") {
            data = Assets::DataUri(uri).Decode();
        } else {
            auto path{ uri.DecodePath() };
            auto size{ std::filesystem::file_size(path) };
            data.resize(size);
            std::ifstream file(path, std::ios::binary);
            file.read(reinterpret_cast<char*>(data.data()), data.size());
        }
        binaryData = std::make_shared<Assets::BinaryData>(data);
    }
    asset->assets.push_back(binaryData);
    asset->SetAssetType(Assets::BinaryData::AssetType);
    asset->SetLoaded(true);
}