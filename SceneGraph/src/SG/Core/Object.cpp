/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-19 00:35:28
*/

#include <SG/Core/Object.hpp>

#include <random>
#include <ostream>

namespace TabGraph::SG {
Object::Object()
{
    static auto s_objectNbr { 0u };
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    _id = dist(gen);
    _name = "Object_" + std::to_string(++s_objectNbr);
}
std::ostream& Object::SerializeData(std::ostream& a_Ostream, const std::string& a_Name, const std::byte* a_Data, const size_t& a_Size)
{
    a_Ostream << a_Name << " = "; a_Ostream.write((char*)a_Data, a_Size); a_Ostream << ";\n";
    return a_Ostream;
}
};
