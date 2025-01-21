#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <Core/Buffer/View.hpp>
#include <Core/Image/Image.hpp>
#include <Core/Material.hpp>
#include <Core/Material/Extension/Base.hpp>
#include <Core/Material/Extension/MetallicRoughness.hpp>
#include <Core/Material/Extension/Sheen.hpp>
#include <Core/Material/Extension/SpecularGlossiness.hpp>
#include <Core/Texture/Sampler.hpp>
#include <Core/Texture/Texture.hpp>
#include <Tools/LazyConstructor.hpp>

#include <iostream>

namespace MSG::Renderer {
MaterialLoader::MaterialLoader()
{
}

std::shared_ptr<Material> MaterialLoader::Load(Renderer::Impl& a_Renderer, Core::Material* a_Material)
{
    Tools::LazyConstructor factory = [this, &a_Renderer, &a_Material] {
        auto material = std::make_shared<Material>(a_Renderer.context);
        material->Set(a_Renderer, *a_Material);
        return material;
    };
    return GetOrCreate(a_Material, factory);
}

std::shared_ptr<Material> MaterialLoader::Update(Renderer::Impl& a_Renderer, Core::Material* a_Material)
{
    auto& material = at(a_Material);
    material->Set(a_Renderer, *a_Material);
    return material;
}
}
