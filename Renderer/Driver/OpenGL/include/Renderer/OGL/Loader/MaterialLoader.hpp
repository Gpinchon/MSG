#pragma once

#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <Tools/ObjectCache.hpp>

namespace TabGraph::Renderer::RAII {
struct Context;
}

namespace TabGraph::Renderer {
struct Impl;
struct Material;
union MaterialUBO;
}

namespace TabGraph::SG {
class Texture;
class TextureSampler;
class Material;
struct BaseExtension;
struct SpecularGlossinessExtension;
}

namespace TabGraph::Renderer {
using MaterialCacheKey = Tools::ObjectCacheKey<SG::Material*>;
using MaterialCache    = Tools::ObjectCache<MaterialCacheKey, std::shared_ptr<Material>>;
class MaterialLoader : MaterialCache {
public:
    MaterialLoader();
    std::shared_ptr<Material> Load(Renderer::Impl& a_Renderer, SG::Material* a_Material);
    std::shared_ptr<Material> Update(Renderer::Impl& a_Renderer, SG::Material* a_Material);
};
}
