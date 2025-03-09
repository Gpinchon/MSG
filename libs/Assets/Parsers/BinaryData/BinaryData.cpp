#include <MSG/Assets/Asset.hpp>
#include <MSG/Assets/Parser.hpp>
#include <MSG/Assets/Uri.hpp>
#include <MSG/Buffer.hpp>
#include <MSG/Debug.hpp>

#include <cassert>
#include <fstream>

namespace MSG::Assets {
std::shared_ptr<Asset> ParseBinaryData(const std::shared_ptr<Asset>& asset)
{
    std::shared_ptr<Buffer> binaryData;
    {
        auto& uri { asset->GetUri() };
        if (uri.GetScheme() == "data") {
            binaryData = std::make_shared<Buffer>(DataUri(uri).Decode());
        } else {
            const auto path { uri.DecodePath() };
            const auto size { std::filesystem::file_size(path) };
            binaryData = std::make_shared<Buffer>(size);
            std::basic_ifstream<char> file;
            file.exceptions(file.exceptions() | std::ios::badbit | std::ios::failbit);
            try {
                file.open(path, std::ios::binary);
                file.read((char*)binaryData->data(), size);
                auto readSize = file.gcount();
                if (readSize != size)
                    errorLog("Read size : " + std::to_string(readSize) + ", expected : " + std::to_string(size));
            } catch (std::ios_base::failure& e) {
                errorLog(path.string() + " : " + e.what());
            }
        }
    }
    asset->AddObject(binaryData);
    asset->SetAssetType("BinaryData");
    asset->SetLoaded(true);
    return asset;
}
}
