#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Assets/Uri.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/ECS/Registry.hpp>

#include <limits>
#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

namespace MSG {
class BufferView;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Assets {
class Asset {
public:
    /// Parsing options for the various types of assets this could contain
    struct {
        struct {
            uint32_t maxWidth   = std::numeric_limits<uint32_t>::max();
            uint32_t maxHeight  = std::numeric_limits<uint32_t>::max();
            float maxPixelValue = std::numeric_limits<float>::max();
        } image;
        struct {
            bool compress = true;
        } texture;
        struct {
            bool generateLODs     = true; // generate LODs if the asset doesn't provide any
            uint8_t lodsNbr       = 3; // number of LODs to generate if applicable
            float lodsCompression = 1 / float(lodsNbr + 1); // the wanted compression level for each lod (default 25%)
            float lodsMaxError    = 100.f; // the approximation level that's considered acceptable for the lods
        } mesh;
    } parsingOptions;
    // Generally the mime type
    PROPERTY(std::string, AssetType, "");
    PROPERTY(std::string, Name, "");
    // The Unique Resource Identifier
    PROPERTY(Uri, Uri, );
    // The ECS registry, generally used to store scenegraphs
    PROPERTY(std::shared_ptr<ECS::DefaultRegistry>, ECSRegistry, nullptr);
    // A vector of objects, could be images, animations, anything really...
    PROPERTY(std::vector<std::shared_ptr<Core::Object>>, Objects, );
    PROPERTY(bool, Loaded, false);

public:
    Asset();
    Asset(const Uri& a_Uri)
        : Asset()
    {
        SetUri(a_Uri);
    }
    Asset(const Asset&) = delete;
    inline std::mutex& GetLock()
    {
        return _lock;
    }

    template <typename T>
    inline auto Get()
    {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : GetObjects()) {
            if (object->IsOfType(typeid(T)))
                objects.push_back(std::static_pointer_cast<T>(object));
        }
        return objects;
    }
    template <typename T>
    inline auto GetByName(const std::string& a_Name)
    {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : Get<T>()) {
            if (std::string(object->GetName()) == a_Name)
                objects.push_back(object);
        }
        return objects;
    }

    template <typename T>
    inline auto GetCompatible()
    {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : GetObjects()) {
            if (object->IsCompatible(typeid(T)))
                objects.push_back(std::static_pointer_cast<T>(object));
        }
        return objects;
    }
    template <typename T>
    inline auto GetCompatibleByName(const std::string& name)
    {
        std::vector<std::shared_ptr<T>> objects;
        for (const auto& object : GetCompatible<T>()) {
            if (object->GetName() == name)
                objects.push_back(object);
        }
        return objects;
    }
    inline void AddObject(std::shared_ptr<Core::Object> a_asset)
    {
        GetObjects().push_back(a_asset);
    }
    /**
     * @brief Merges the Objects, but not the ECS registries which are not mergeable
     * In order to share ECS accross assets use SetECSRegistry
     */
    inline void MergeObjects(std::shared_ptr<Asset> a_asset)
    {
        GetObjects().insert(GetObjects().end(), a_asset->GetObjects().begin(), a_asset->GetObjects().end());
    }

private:
    std::mutex _lock;
};
}
