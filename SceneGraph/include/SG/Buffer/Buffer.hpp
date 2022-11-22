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
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

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
	PROPERTY(std::vector<std::byte>, RawData, 0);
public:
	Buffer();
	inline Buffer(const std::string& a_Name) : Buffer() { SetName(a_Name); }
	inline Buffer(const size_t& a_Size) : Buffer() { resize(a_Size); }
	inline Buffer(const std::vector<std::byte>& a_RawData) : Buffer() { SetRawData(a_RawData); }
	inline void resize(const size_t& a_Size) { GetRawData().resize(a_Size); }
	inline auto size() { return GetRawData().size(); }
	inline auto data() { return GetRawData().data(); }
	inline auto size() const { return GetRawData().size(); }
	inline auto data() const { return GetRawData().data(); }
	inline auto& at(size_t a_Index) { return GetRawData().at(a_Index); }
	inline auto begin() { return GetRawData().begin(); }
	inline auto end() { return GetRawData().end(); }
	inline auto& at(size_t a_Index) const { return GetRawData().at(a_Index); }
	inline auto begin() const { return GetRawData().begin(); }
	inline auto end() const { return GetRawData().end(); }
	template<typename T>
	inline void push_back(const T& a_Value) {
		const auto offset = size();
		resize(offset + sizeof(T));
		std::memcpy(data() + offset, &a_Value, sizeof(T));
	}
};
}