#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Image/Pixel.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class BufferView;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Image : public Inherit<Object, Image> {
public:
	enum class Type {
		Unknown, Image1D, Image2D, Image3D, MaxValue
	};
	enum class SamplingFilter {
		Nearest, Bilinear, MaxValue
	};
	PROPERTY(Type, Type, Type::Unknown);
	PROPERTY(Pixel::Description, PixelDescription, );
	PROPERTY(glm::ivec3, Size, 0);
	PROPERTY(std::shared_ptr<BufferView>, BufferView, );

public:
	Image();
	Image(const Type& a_Type, const Pixel::Description& a_PixelDesc, const glm::ivec3 a_Size, const std::shared_ptr<BufferView>& a_BufferView = {}) : Image()
	{
		SetType(a_Type);
		SetPixelDescription(a_PixelDesc);
		SetSize(a_Size);
		SetBufferView(a_BufferView);
	}
	/**
	 * @brief Fetches a color from the coordinates, asserts thad _data is not empty
	 * @param texCoord the texture coordinate to fetch the color from
	 * @param filter the filtering to be used for sampling, default is nearest
	 * @return the unpacked color
	*/
	Pixel::Color GetColor(const glm::vec3& texCoord, SamplingFilter filter = SamplingFilter::Nearest);
	/**
	 * @brief Sets the pixel corresponding to tesCoord to the specified color
	 * @param texCoord the texture coordinates to be set
	 * @param color : the new color of this pixel
	*/
	void SetColor(const Pixel::Coord& texCoord, const Pixel::Color& color);
private:
	std::byte* Image::_GetPointer(const Pixel::Coord& texCoord);
};
}