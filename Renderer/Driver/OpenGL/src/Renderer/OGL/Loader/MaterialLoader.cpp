#include <Renderer/OGL/Loader/MaterialLoader.hpp>
#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/RAII/TextureSampler.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/Base.hpp>
#include <SG/Core/Material/Extension/MetallicRoughness.hpp>
#include <SG/Core/Material/Extension/Sheen.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <Tools/LazyConstructor.hpp>

#include <iostream>

namespace TabGraph::Renderer {
MaterialLoader::MaterialLoader()
{
}

std::shared_ptr<Material> MaterialLoader::Load(Renderer::Impl& a_Renderer, SG::Material* a_Material)
{
    Tools::LazyConstructor factory = [this, &a_Renderer, &a_Material] {
        auto material = std::make_shared<Material>(a_Renderer.context);
        material->Set(a_Renderer, *a_Material);
        return material;
    };
    return GetOrCreate(a_Material, factory);
}

std::shared_ptr<Material> MaterialLoader::Update(Renderer::Impl& a_Renderer, SG::Material* a_Material)
{
    auto& material = at(a_Material);
    material->Set(a_Renderer, *a_Material);
    return material;
}
}
