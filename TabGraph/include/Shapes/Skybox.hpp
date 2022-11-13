/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:26
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>
#include <Shapes/Shape.hpp>

#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Textures {
class TextureCubemap;
}
namespace Shader {
class Program;
}
namespace Renderer {
class SkyboxRenderer;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Skybox : public Core::Inherit<Shape, Skybox> {
public:
    PROPERTY(std::shared_ptr<Textures::TextureCubemap>, Texture, nullptr);

public:
    Skybox(const std::string& name);
    Skybox(const std::string& name, std::shared_ptr<Textures::TextureCubemap> color);
    Skybox(const Skybox& other);
    void Load();
    void Unload();
};
}

