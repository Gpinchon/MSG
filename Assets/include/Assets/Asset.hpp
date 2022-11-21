#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Assets/Uri.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

namespace TabGraph::SG {
class BufferView;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset {
public:
    ///Parsing options for the various types of assets this could contain
    struct {
        struct {
            bool useBufferView{ false };
        } data;
        struct {
            int32_t maximumResolution { -1 };
        } image;
        struct {
            bool compress { true };
            uint8_t compressionQuality { 255 };
        } texture;
    } parsingOptions;
    PROPERTY(std::string, AssetType, "");
    PROPERTY(std::string, Name, "");
    PROPERTY(Uri, Uri, );
    //Used for data assets when data.useBufferView is true
    PROPERTY(std::shared_ptr<SG::BufferView>, BufferView, nullptr);
    PROPERTY(bool, Loaded, false);

public:
    Asset();
    Asset(const Uri& a_Uri) : Asset() { SetUri(a_Uri); }
    Asset(const Asset&) = delete;
    inline std::mutex& GetLock() {
        return _lock;
    }

    template<typename T>
    inline auto Get() {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : assets) {
            if (object->IsOfType(typeid(T)))
                objects.push_back(std::static_pointer_cast<T>(object));
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

    template<typename T>
    inline auto GetCompatible() {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : assets) {
            if (object->IsCompatible(typeid(T)))
                objects.push_back(std::static_pointer_cast<T>(object));
        }
        return objects;
    }
    template<typename T>
    inline auto GetCompatibleByName(const std::string& name) {
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
    std::vector<std::shared_ptr<SG::Object>> assets;

private:
    std::mutex _lock;
};
}
