#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>

#include <map>
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class MaterialParameters;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Material : public Inherit<Object, Material> {
public:
	Material(const std::string& a_Name) : Inherit(a_Name) {}
	template<typename Parameters>
	void AddParameters(const Parameters& a_Extension) {
		_parameters[a_Extension.GetName()] = std::make_unique<Parameters>(a_Extension);
	}
	template<typename Parameters>
	auto& GetParameter(const std::string& a_Name) const {
		auto& parameter = _parameters.at(a_Name);
		assert(parameter->IsCompatible(typeid(Parameters)));
		return parameter;
	}
	auto& GetParameters() const { return _parameters; }

private:
	std::map<std::string, std::unique_ptr<MaterialParameters>> _parameters;
};
}