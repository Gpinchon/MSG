#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Buffer/View.hpp>
#include <Image.hpp>
#include <Material.hpp>
#include <Material/Extension/Base.hpp>
#include <Material/Extension/MetallicRoughness.hpp>
#include <Material/Extension/Sheen.hpp>
#include <Material/Extension/SpecularGlossiness.hpp>
#include <Sampler.hpp>
#include <Texture.hpp>
#include <Tools/LazyConstructor.hpp>

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
