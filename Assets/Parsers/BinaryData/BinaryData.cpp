/*
 * @Author: gpinchon
 * @Date:   2021-02-03 17:50:02
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-02-03 17:50:55
 */

#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Uri.hpp>

#include <SG/Core/Buffer/Buffer.hpp>

#include <Tools/Debug.hpp>

#include <cassert>
#include <fstream>

namespace TabGraph::Assets {
std::shared_ptr<Asset> ParseBinaryData(const std::shared_ptr<Asset>& asset)
{
    std::shared_ptr<SG::Buffer> binaryData;
    {
        auto& uri { asset->GetUri() };
        if (uri.GetScheme() == "data") {
            binaryData = std::make_shared<SG::Buffer>(DataUri(uri).Decode());
        } else {
            const auto path { uri.DecodePath() };
            const auto size { std::filesystem::file_size(path) };
            binaryData = std::make_shared<SG::Buffer>(size);
            std::basic_ifstream<std::byte> file;
            file.exceptions(file.exceptions() | std::ios::badbit | std::ios::failbit);
            try {
                file.open(path, std::ios::binary);
                file.read(binaryData->data(), size);
                auto readSize = file.gcount();
                if (readSize != size)
                    debugLog("Read size : " + std::to_string(readSize) + ", expected : " + std::to_string(size));
            } catch (std::ios_base::failure& e) {
                debugLog(path.string() + " : " + e.what());
            }
        }
    }
    asset->AddObject(binaryData);
    asset->SetAssetType("BinaryData");
    asset->SetLoaded(true);
    return asset;
}
}
