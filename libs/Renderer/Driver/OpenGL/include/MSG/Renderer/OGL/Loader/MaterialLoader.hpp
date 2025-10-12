#pragma once

#include <MSG/Tools/ObjectCache.hpp>

#include <memory>

namespace Msg {
class OGLContext;
}

namespace Msg::Renderer {
class Impl;
class Material;
struct MaterialUBO;
}

namespace Msg {
class Material;
}

namespace Msg::Renderer {
using MaterialCacheKey = Tools::ObjectCacheKey<Msg::Material*>;
using MaterialCache    = Tools::ObjectCache<MaterialCacheKey, std::shared_ptr<Material>>;
class MaterialLoader : MaterialCache {
public:
    MaterialLoader();
    std::shared_ptr<Material> Load(Renderer::Impl& a_Renderer, Msg::Material* a_Material);
    std::shared_ptr<Material> Update(Renderer::Impl& a_Renderer, Msg::Material* a_Material);
};
}
