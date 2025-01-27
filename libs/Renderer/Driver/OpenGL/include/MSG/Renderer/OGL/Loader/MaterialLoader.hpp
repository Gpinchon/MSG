#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace MSG {
class OGLContext;
}

namespace MSG::Renderer {
class Impl;
class Material;
struct MaterialUBO;
}

namespace MSG {
class Material;
}

namespace MSG::Renderer {
using MaterialCacheKey = Tools::ObjectCacheKey<MSG::Material*>;
using MaterialCache    = Tools::ObjectCache<MaterialCacheKey, std::shared_ptr<Material>>;
class MaterialLoader : MaterialCache {
public:
    MaterialLoader();
    std::shared_ptr<Material> Load(Renderer::Impl& a_Renderer, MSG::Material* a_Material);
    std::shared_ptr<Material> Update(Renderer::Impl& a_Renderer, MSG::Material* a_Material);
};
}
