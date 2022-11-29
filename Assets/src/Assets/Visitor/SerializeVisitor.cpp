#include <Assets/Visitor/SerializeVisitor.hpp>
#include <Assets/Uri.hpp>
#include <Tools/Base.hpp>

#include <SG/Node/Node.hpp>
#include <SG/Node/NodeGroup.hpp>
#include <SG/Buffer/Buffer.hpp>
#include <SG/Buffer/View.hpp>
#include <SG/Buffer/Accessor.hpp>

namespace TabGraph::Assets {
static inline auto& SerializePropertyName(std::ostream& a_Ostream, const std::string& a_Name)
{
    return a_Ostream << "    \"" << a_Name << "\" : ";
}

static inline auto& SerializeProperty(std::ostream& a_Ostream, const std::string& a_Name, const uint32_t& a_Value) {
    return SerializePropertyName(a_Ostream, a_Name) << a_Value << ",\n";;
}

static inline auto& SerializeProperty(std::ostream& a_Ostream, const std::string& a_Name, const std::string& a_Value) {
    return SerializePropertyName(a_Ostream, a_Name) << '\"' << a_Value << "\",\n";
}

static inline auto& SerializeData(std::ostream& a_Ostream, const std::string& a_Name, const std::byte* a_Data, const size_t& a_Size)
{
    DataUri uri;
    uri.SetMime("application/octet-stream");
    uri.SetBase64(true);
    uri.SetData(Tools::Base64::Encode({ a_Data , a_Data + a_Size}));
    return SerializeProperty(a_Ostream, a_Name, std::string(uri));
}

static inline auto& Serialize(std::ostream& a_Ostream, const SG::Buffer& a_Buffer) {
    return SerializeData(a_Ostream, "Data", a_Buffer.data(), a_Buffer.size());
}

static inline auto& Serialize(std::ostream& a_Ostream, const SG::BufferView& a_BufferView) {
    SerializeProperty(a_Ostream, "Buffer", a_BufferView.GetBuffer()->GetId());
    SerializeProperty(a_Ostream, "ByteLength", a_BufferView.GetByteLength());
    SerializeProperty(a_Ostream, "ByteOffset", a_BufferView.GetByteOffset());
    return SerializeProperty(a_Ostream, "ByteStride", a_BufferView.GetByteStride());
}

static inline auto& Serialize(std::ostream& a_Ostream, const SG::BufferAccessor& a_Accessor) {
    SerializeProperty(a_Ostream, "ByteOffset", a_Accessor.GetByteOffset());
    SerializeProperty(a_Ostream, "Size", a_Accessor.GetSize());
    SerializeProperty(a_Ostream, "ComponentTypeSize", a_Accessor.GetComponentTypeSize());
    SerializeProperty(a_Ostream, "ComponentNbr", a_Accessor.GetComponentNbr());
    SerializeProperty(a_Ostream, "ComponentType", (int)a_Accessor.GetComponentType());
    SerializeProperty(a_Ostream, "BufferView", a_Accessor.GetBufferView()->GetId());
    return SerializeProperty(a_Ostream, "Normalized", a_Accessor.GetNormalized());
}

static inline auto& Serialize(std::ostream& a_Ostream, const SG::Node& a_Node) {
    if (a_Node.GetParent()) SerializeProperty(a_Ostream, "Parent", a_Node.GetParent()->GetId());
    return a_Ostream;
}

static inline auto& Serialize(std::ostream& a_Ostream, const SG::NodeGroup& a_NodeGroup) {
    SerializePropertyName(a_Ostream, "Children") << '[';
    for (auto& child : a_NodeGroup.GetChildren())
        a_Ostream << child->GetId() << ',';
    return a_Ostream << "],\n";
}

void SerializeVisitor::_Visit(SG::Object& a_Object) {
    _ostream << "{\n";
    SerializeProperty(_ostream, "Type", a_Object.GetClassName());
    SerializeProperty(_ostream, "Id", a_Object.GetId());
    SerializeProperty(_ostream, "Name", a_Object.GetName());
    if (a_Object.IsCompatible(typeid(SG::Node))) Serialize(_ostream, (SG::Node&)a_Object);
    if (a_Object.IsCompatible(typeid(SG::NodeGroup))) Serialize(_ostream, (SG::NodeGroup&)a_Object);
    if (a_Object.IsCompatible(typeid(SG::Buffer))) Serialize(_ostream, (SG::Buffer&)a_Object);
    if (a_Object.IsCompatible(typeid(SG::BufferAccessor))) Serialize(_ostream, (SG::BufferAccessor&)a_Object);
    if (a_Object.IsCompatible(typeid(SG::BufferView))) Serialize(_ostream, (SG::BufferView&)a_Object);
    _ostream << "},\n";
}
void SerializeVisitor::_Visit(SG::Node& a_Node) {
    _Visit(reinterpret_cast<SG::Object&>(a_Node));
}
void SerializeVisitor::_Visit(SG::NodeGroup& a_NodeGroup) {
    _Visit(reinterpret_cast<SG::Node&>(a_NodeGroup));
    
}
}
