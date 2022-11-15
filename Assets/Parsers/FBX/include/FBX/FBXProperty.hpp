/*
* @Author: gpinchon
* @Date:   2019-08-10 11:23:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:10
*/

#pragma once

#include <FBX/FBXObject.hpp>
#include <stdint.h>
#include <string>
#include <variant>
#include <zlib.h>

namespace FBX {
typedef std::variant<Byte*, char*, float*, double*, int32_t*, int64_t*>
    ArrayData;

struct Array {
    uint32_t length;
    uint32_t encoding;
    uint32_t compressedLength;
    ArrayData data;
    //template <typename T>
    //operator T() const { return std::get<T>(data); }
    operator Byte*() const { return std::get<Byte*>(data); }
    operator char*() const { return std::get<char*>(data); }
    operator float*() const { return std::get<float*>(data); }
    operator double*() const { return std::get<double*>(data); }
    operator int32_t*() const { return std::get<int32_t*>(data); }
    operator int64_t*() const { return std::get<int64_t*>(data); }
    operator std::string() const { return std::get<char*>(data); };
};

typedef std::variant<Byte, float, double, int16_t, int32_t, int64_t, Array>
    PropertyData;

class Property : public Object {
public:
    virtual ~Property() = default;
    static std::shared_ptr<Property> Create();
    unsigned char typeCode;
    PropertyData data;
    virtual void Print() const;
    //template <typename T>
    //operator T() const { return std::get<T>(data); }
    operator Byte() const { return std::get<Byte>(data); }
    operator float() const { return std::get<float>(data); }
    operator double() const { return std::get<double>(data); }
    operator int16_t() const { return std::get<int16_t>(data); }
    operator int32_t() const { return std::get<int32_t>(data); }
    operator int64_t() const { return std::get<int64_t>(data); }
    operator Array() const { return std::get<Array>(data); }
    operator std::string() const { return std::get<Array>(data); }
};
} // namespace FBX
