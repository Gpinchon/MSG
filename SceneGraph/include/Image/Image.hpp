#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Light/Light.hpp>
#include <SphericalHarmonics.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>
#include <Core/PixelUtils.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

namespace TabGraph::SG {
class Image : public Core::Inherit<Core::Object, Image> {
public:
	enum class Type {
		Unknown, Image1D, Image2D, Image3D, MaxValue
	}
	PROPERTY(Type, Type, Type::Unknown);
	PROPERTY(Pixel::Description, PixelDescription, );
public:
	enum class Type {
		Unknown, Image1D, Image2D, Image3D, MaxValue
	}
	Image(const Type& a_Type) {
		SetType(a_Type);
	}
}
}