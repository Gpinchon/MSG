#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

namespace MSG::Renderer {
class Impl;
class Material;
class Context;
struct MaterialUBO;
}

namespace MSG::SG {
class Texture;
class Sampler;
class Material;
struct BaseExtension;
struct SpecularGlossinessExtension;
}

namespace MSG::Renderer {
using MaterialCacheKey = Tools::ObjectCacheKey<SG::Material*>;
using MaterialCache    = Tools::ObjectCache<MaterialCacheKey, std::shared_ptr<Material>>;
class MaterialLoader : MaterialCache {
public:
    MaterialLoader();
    std::shared_ptr<Material> Load(Renderer::Impl& a_Renderer, SG::Material* a_Material);
    std::shared_ptr<Material> Update(Renderer::Impl& a_Renderer, SG::Material* a_Material);
};
}
