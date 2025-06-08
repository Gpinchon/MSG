#include <MSG/Renderer/OGL/Loader/MaterialLoader.hpp>
#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/Image.hpp>
#include <MSG/Material.hpp>
#include <MSG/Material/Extension/Base.hpp>
#include <MSG/Material/Extension/MetallicRoughness.hpp>
#include <MSG/Material/Extension/Sheen.hpp>
#include <MSG/Material/Extension/SpecularGlossiness.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

#include <iostream>

namespace MSG::Renderer {
MaterialLoader::MaterialLoader()
{
}

std::shared_ptr<Material> MaterialLoader::Load(Renderer::Impl& a_Renderer, MSG::Material* a_Material)
{
    Tools::LazyConstructor factory = [this, &a_Renderer, &a_Material] {
        auto material = std::make_shared<Material>(a_Renderer.context);
        material->Set(a_Renderer, *a_Material);
        return material;
    };
    return GetOrCreate(a_Material, factory);
}

std::shared_ptr<Material> MaterialLoader::Update(Renderer::Impl& a_Renderer, MSG::Material* a_Material)
{
    auto& material = at(a_Material);
    material->Set(a_Renderer, *a_Material);
    return material;
}
}
