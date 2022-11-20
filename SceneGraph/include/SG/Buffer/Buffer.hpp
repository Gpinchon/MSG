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
	Buffer();
	inline Buffer(const std::string& a_Name) : Buffer() { SetName(a_Name); }
	inline Buffer(const size_t& a_Size) : Buffer() { resize(a_Size); }
	inline Buffer(const std::vector<std::byte>& a_RawData) : Buffer() { _rawData = a_RawData; }
	inline void resize(const size_t& a_Size) { _rawData.resize(a_Size); }
	inline auto size() { return _rawData.size(); }
	inline auto data() { return _rawData.data(); }
	inline auto size() const { return _rawData.size(); }
	inline auto data() const { return _rawData.data(); }
	inline auto& begin() { return _rawData.begin(); }
	inline auto& end() { return _rawData.end(); }
	inline auto& begin() const { return _rawData.begin(); }
	inline auto& end() const { return _rawData.end(); }
	template<typename T>
	inline void push_back(const T& a_Value) {
		const auto offset = size();
		resize(offset + sizeof(T));
		std::memcpy(data() + offset, &a_Value, sizeof(T));
	}
	inline virtual std::ostream& Serialize(std::ostream& a_Ostream) const override {
		Inherit::Serialize(a_Ostream);
		SerializeProperty(a_Ostream, "Size", size());
		SerializeData(a_Ostream, "Data", data(), size());
		return a_Ostream;
	}

private:
	std::vector<std::byte> _rawData;
};
}