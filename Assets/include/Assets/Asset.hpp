#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Assets/Uri.hpp>
#include <Config.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset : public Core::Inherit<Core::Object, Asset> {
public:
    ///Parsing options for the various types of assets this could contain
    struct {
        struct {
            int32_t maximumResolution { Config::Global().Get("ImagesMaximumResolution", -1) };
        } image;
        struct {
            bool compress { bool(Config::Global().Get("TexturesCompressed", 1)) };
            uint8_t compressionQuality { uint8_t(Config::Global().Get("TexturesCompressionQuality", 0.25f) * 255) };
        } texture;
    } parsingOptions;
    PROPERTY(std::string, AssetType, "");
    PROPERTY(Uri, Uri, );

public:
    Asset() = default;
    Asset(const Uri&);
    Asset(const Asset&) = delete;
    inline bool GetLoaded()
    {
        return _loaded.load();
    }
    inline void SetLoaded(bool loaded)
    {
        _loaded.store(loaded);
    }
    inline std::mutex& GetLock()
    {
        return _lock;
    }
    template<typename T>
    inline auto Get() {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : assets) {
            if (object->IsCompatible(typeid(T)))
                objects.push_back(std::static_pointer_cast<T>(object));
        }
        return objects;
    }
    inline auto GetByName(const std::string& name) {
        std::vector<std::shared_ptr<Core::Object>> objects;
        for (const auto& object : assets) {
            if (object->GetName() == name)
                objects.push_back(object);
        }
        return objects;
    }

    template<typename T>
    inline auto GetByName(const std::string& name) {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : Get<T>()) {
            if (object->GetName() == name)
                objects.push_back(object);
        }
        return objects;
    }
    inline void Add(std::shared_ptr<Asset> a_asset) {
        assets.insert(assets.end(), a_asset->assets.begin(), a_asset->assets.end());
    }
    std::vector<std::shared_ptr<Core::Object>> assets;

private:
    std::atomic<bool> _loaded { false };
    std::mutex _lock;
};
}
