#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Material/Extension.hpp>

#include <glm/vec3.hpp>

#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Material : public Core::Inherit<Core::Object, Material> {
public:
    Material();
    Material(const std::string& a_Name)
        : Inherit(a_Name)
    {
    }
    template <typename Extension>
    bool HasExtension() const
    {
        return _parameters.find(typeid(Extension)) != _parameters.end();
    }
    template <typename Extension>
    void AddExtension(const Extension& a_Extension)
    {
        _parameters[typeid(Extension)] = std::make_shared<Extension>(a_Extension);
    }
    template <typename Extension>
    auto& GetExtension() const
    {
        auto ptr = _parameters.at(typeid(Extension));
        return *std::static_pointer_cast<Extension>(ptr);
    }
    auto& GetExtensions() const { return _parameters; }

private:
    std::map<std::type_index, std::shared_ptr<MaterialExtension>> _parameters;
};
}
