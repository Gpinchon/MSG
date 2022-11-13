/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

#include "Surface/Skybox.hpp"
#include "Renderer/Surface/SkyboxRenderer.hpp"
#include "Texture/TextureCubemap.hpp"

Skybox::Skybox(const std::string& name)
    : Component(name)
    , _renderer(new Renderer::SkyboxRenderer(*this))
{
}

Skybox::Skybox(const std::string& name, std::shared_ptr<TextureCubemap> color)
    : Skybox(name)
{
    SetTexture(color);
}

Skybox::Skybox(const Skybox& other)
    : _renderer(new Renderer::SkyboxRenderer(*this))
{
    SetTexture(other.GetTexture());
}

void Skybox::Load()
{
    GetTexture()->Load();
}

void Skybox::Unload()
{
    GetTexture()->Unload();
}

Renderer::SkyboxRenderer& Skybox::GetRenderer()
{
    return *_renderer;
}
