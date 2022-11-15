#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Light/Light.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Image/Pixel.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

namespace TabGraph::SG {
class Image : public Inherit<Object, Image> {
public:
	enum class Type {
		Unknown, Image1D, Image2D, Image3D, MaxValue
	};
	PROPERTY(Type, Type, Type::Unknown);
	PROPERTY(Pixel::Description, PixelDescription, );
	PROPERTY(std::vector<std::byte>, Data, );

public:
	Image(const Type& a_Type, const std::vector<std::byte>& a_Data = {})
	{
		SetData(a_Data);
		SetType(a_Type);
	}
};
}