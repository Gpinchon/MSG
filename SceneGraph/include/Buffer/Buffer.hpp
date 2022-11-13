/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:43
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <memory>
#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
/**
 * @brief this is just a buffer of raw bytes
 */
class Buffer : public Inherit<Object, Buffer> {
public:
	Buffer(const size_t& a_Size) : _rawData(a_Size) {}
	Buffer(const std::vector<std::byte> a_RawData) : _rawData(_rawData) {}
	inline auto& GetData() { return _rawData; }
	inline auto& begin() { return _rawData.begin(); }
	inline auto& end() { return _rawData.end(); }
	inline auto& begin() const { return _rawData.begin(); }
	inline auto& end() const { return _rawData.end(); }

private:
	std::vector<std::byte> _rawData;
};
}