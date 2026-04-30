#pragma once

#include <MSG/Debug.hpp>

#include <memory>
#include <unordered_map>

namespace Renderer {
struct GPUResource {
    size_t memorySize;
    std::shared_ptr<void> resource;
};
class GPUResourceManager {
public:
    template <typename T, typename... Args>
    std::pair<size_t, std::shared_ptr<T>> Allocate(const size_t& a_GPUMemorySize, Args&&... a_Args);
    void Free(const size_t& a_ResourceID);

private:
    size_t _memoryInUse       = 0;
    size_t _currentResourceID = -1u;
    std::unordered_map<size_t, GPUResource> _resources;
};

template <typename T, typename... Args>
inline std::pair<size_t, std::shared_ptr<T>> Renderer::GPUResourceManager::Allocate(const size_t& a_GPUMemorySize, Args&&... a_Args)
{
    auto resourceID  = _currentResourceID++;
    auto resourcePtr = std::make_shared<T>(std::forward(a_Args)...);
    GPUResource resource;
    resource.memorySize = a_GPUMemorySize;
    resource.resource   = resourcePtr;
    _memoryInUse += a_GPUMemorySize;
    _resources.insert({ resourceID, resource });
    return std::make_pair(resourceID, resourcePtr);
}

inline void Renderer::GPUResourceManager::Free(const size_t& a_ResourceID)
{
    auto& resourceItr = _resources.find(a_ResourceID);
    MSGCheckErrorFatal(resourceItr == _resources.end(), "Could not find requested resource !");
    MSGCheckErrorFatal(resourceItr->second.resource.use_count() > 1, "Resource is still in use !");
    _memoryInUse -= resourceItr->second.memorySize;
    _resources.erase(resourceItr);
}
}